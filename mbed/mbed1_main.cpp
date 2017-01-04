/*
* mbed1_ main.cpp
* Created by K.K
* Original source code is https://github.com/3116igbt/16hacku_zabuton
*
* mbed1_main.cpp is main source code that measurement acceleration and pressure, 
* contorol speaker and vibration.
*/


#include "mbed.h"
#include "ADXL345_I2C.h"
#include "wave_player.h"
#include "rtos.h"
#include "SDFileSystem.h"

#define TIME_FOR_SOUND 20000                       // 音を鳴らすまでの時間[msec]
#define TIME_FOR_MOTOR 5000                       // モータを動かすまでの時間[msec]
#define CYCLE_FOR_PRESSURE 300                  // 圧力サンプリング周期[sec]
#define CYCLE_FOR_ACCELERATION 10               // 加速度サンプリング周期[sec]
#define THRESHOLD_FOR_PRESSURE 0.7              // 着席判定閾値[V]
#define TIMESLOT_LENGTH_FOR_PRESSURE 3          // 圧力センサタイムスロット長
#define TIMESLOT_LENGTH_FOR_ACCELERATION 200    // 加速度センサタイムスロット長
#define MIN_VARIANCE 15 // イライラレベル下限
#define MAX_VARIANCE 120 // イライラレベル上限
#define ERR_VARIANCE 300 // エラー基準値
#define AWAY 0x01
#define SEAT 0x02
#define LOW 0
#define MIDDLE 1
#define HIGH 2
#define LV_MID 50 // イライラレベル中 閾値
#define LV_HIGH 80 // イライラレベル大 閾値
#define LV_ERR 300 // イライラレベル判別不能 閾値
#define START 1
#define STOP 0

ADXL345_I2C accelerometer(p9, p10);
RawSerial pc(USBTX, USBRX);
RawSerial serial(p13, p14);
DigitalOut seatFlag(p22);
DigitalIn soundFlag(p23);
DigitalOut levels[6] = {
  DigitalOut(p24), DigitalOut(p25), DigitalOut(p26), DigitalOut(p27), DigitalOut(p28), DigitalOut(p29)};
int flag;

SDFileSystem sd(p5, p6, p7, p8, "sd");
AnalogOut wav_out(p18); // スピーカー出力
wave_player waver(&wav_out);
AnalogIn press_in(p15); // 圧力センサ入力
DigitalOut motor_out(p20); // モータ作動

void sound(void const *argument){ // 音を鳴らす
  FILE *fp;
  if((int)argument == 1){
    printf("ir_sound\r\n");
    while(1){
      while(soundFlag.read() == STOP){}
      fp = fopen("/sd/hotaru_1_4.wav", "r");
      while(soundFlag == START){
        waver.play(fp);
        fseek(fp, 0, SEEK_SET);
      }
      fclose(fp);
    }
  }else{
    printf("sound start\r\n");
    fp = fopen("/sd/hotaru_1_4.wav", "r");
    while(press_in.read() < THRESHOLD_FOR_PRESSURE){ //立ち上がるまで
      fseek(fp, 0, SEEK_SET);
    }
    fclose(fp);
  }
}

void motor(void const *argument){ // モータを動かす
  printf("motor start\r\n");
  motor_out = 1;
  while(press_in.read() < THRESHOLD_FOR_PRESSURE){} // 立ち上がったら
  motor_out = 0;
}

void cushion_control(void const *argument){
  RtosTimer sound_timer(sound, osTimerOnce, (void *)0);
  RtosTimer motor_timer(motor, osTimerOnce);
  while(1){
    Thread::signal_wait(SEAT); // 着席シグナル待ち
    seatFlag = 1;
    printf("sitting\r\n");
    sound_timer.start(TIME_FOR_SOUND);
    motor_timer.start(TIME_FOR_MOTOR);
    Thread::signal_wait(AWAY); // 離席シグナル待ち
    printf("away from cushion\r\n");
    seatFlag = 0;
    sound_timer.stop();
    motor_timer.stop();
  }
}

void pressure_sampling(void const *argument) { // 圧力サンプリング
  float average;
  int i;
  Thread control_thread(cushion_control);
  while(1) {
    for(i=0;i<TIMESLOT_LENGTH_FOR_PRESSURE;i++){
      average += press_in.read();
      Thread::wait(CYCLE_FOR_PRESSURE);
    }
    average = average / TIMESLOT_LENGTH_FOR_PRESSURE;
    if(average < THRESHOLD_FOR_PRESSURE){
      control_thread.signal_clr(AWAY);
      control_thread.signal_set(SEAT); // 着席シグナル
    }else{
      control_thread.signal_clr(SEAT);
      control_thread.signal_set(AWAY); // 離席シグナル
    }
  }
}

void acceleration_sampling(void const *argument){ // 加速度サンプリング
  int total = 0;
  long int average = 0;
  int z_value;
  long int variance = 0;
  int i;
  int level;
  int x = 32;
  
  // 加速度計測器セットアップ 
  accelerometer.setPowerControl(0x00);
  accelerometer.setDataFormatControl(0x0B);
  accelerometer.setDataRate(ADXL345_3200HZ);
  accelerometer.setPowerControl(0x08);
  
  for(i=0;i<TIMESLOT_LENGTH_FOR_ACCELERATION;i++){
    total += accelerometer.getAz();
  }
  average = total / TIMESLOT_LENGTH_FOR_ACCELERATION;
  total = 0;
  while (1){
    for(i=0;i<TIMESLOT_LENGTH_FOR_ACCELERATION;i++){
      Thread::wait(CYCLE_FOR_ACCELERATION);
      z_value = accelerometer.getAz();
      total += z_value;
      variance += (z_value - average) * (z_value - average);
    }
    average = total / TIMESLOT_LENGTH_FOR_ACCELERATION;
    variance = variance / TIMESLOT_LENGTH_FOR_ACCELERATION;
    total = 0;
    // イライラレベル判定
    printf("variance = %d, average = %d\r\n ", variance, average);
    if(variance > 0 && variance < ERR_VARIANCE){
      if(variance > MAX_VARIANCE){
        variance = MAX_VARIANCE;
      }else if(variance < MIN_VARIANCE){
        variance = MIN_VARIANCE;
      }
      level = (variance - MIN_VARIANCE) * 63 / (MAX_VARIANCE - MIN_VARIANCE);
      printf("iraira level = %d\r\n", level);
      for(i=0;i<6;i++){
        if(level >= x){
          levels[i] = 1;
          level -= x;
        }else{
          levels[i] = 0;
        }
        x /= 2;
      }
    }
  }
}

int main(){
  printf("start\r\n");
  seatFlag = 0;
  int i;
  for(i=0;i<6;i++){
    levels[i] = 0;
  }
  /* 計測開始 */
  Thread pressure_thread(pressure_sampling);
  Thread acceleration_thread(acceleration_sampling);
  Thread sound_thread(sound, (void *)1);
  while(1){
    wait(osWaitForever);
  }
}
