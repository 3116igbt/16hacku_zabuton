/*
* mbed2_ main.cpp
* Created by 3116igbt
* Original source code is https://github.com/3116igbt/16hacku_zabuton
*
* mbed2_main.cpp is main source code that telecommunicate with webserver.
*/


#include "mbed.h"
#include "CushionSock.h"

CushionSock *cs;
//Serial rs(p28,p27);//tx,rx
DigitalIn s_sit(p28);
DigitalOut s_act(p27);
DigitalIn s_frust[]={p21,p22,p23,p24,p25,p26};

/*void fromSerial(){
    
    char ch=rs.getc();
    printf("serial recv value %c\n",ch);
    switch(ch){
        case 'D':
            printf("recv sitdown\n");
            cs->mess_send(cs->MESS_SIT,"down");
            break;
        case 'U':
            printf("recv standup\n");
            cs->mess_send(cs->MESS_SIT,"up");
            break;
        case 'F':
            char value=rs.getc();
            char buf[10];
            sprintf(buf,"%d",value);
            printf("recv frust %s\n",buf);
            cs->mess_send(cs->MESS_SIT,buf);
            break;
    }
}*/

int main() {
    Timer timer_sit;
    Timer timer_frust;
    int sitstatus=0;
    s_act=0;
    int frustvalue=0;
    
    cs=new CushionSock();
    cs->init("ws://www.example.jp:12020/test");
    printf("connect start\n");
    cs->connect();
    printf("connect");
    
    //rs.attach(fromSerial, Serial::RxIrq);

    char msg[64];
    timer_sit.start();
    timer_frust.start();
    while(1) {
        Net::poll();
        if(timer_sit.read_ms()>=1000){
            int buf=s_sit.read();
            printf("sitstatus %d\n",buf);
            if(buf!=sitstatus){
                sitstatus=buf;
                printf("message sit %d\n",sitstatus);
                if(sitstatus==1){
                    cs->mess_send(cs->MESS_SIT,"up");
                }else{
                    cs->mess_send(cs->MESS_SIT,"down");
                }
            }
            timer_sit.reset();
        }
        
        if(timer_frust.read_ms()>=5000){
            printf("ok");
            int buf=0;
            int power[]={0,2,4,8,16,32};
            for(int i=0;i<6;i++){
               buf+=s_frust[i].read()*power[i];
               printf("ok");
            }
            char cbuf[15];
            frustvalue=(int)((double)buf/63*100);
            printf("%d",frustvalue);
            sprintf(cbuf,"%d",frustvalue);
            printf("recv frust %s\n",cbuf);
            cs->mess_send(cs->MESS_FRUST,cbuf);
            timer_frust.reset();
        }
        
        int type = cs->mess_recv(msg);
        if (type >= 0) {
            printf("messtype:%d rcv: %s\r\n",type, msg);
            if(type == cs->MESS_START){
                printf("put A\n");
                s_act=1;
            }else if(type == cs->MESS_STOP){
                printf("put S\n");
                s_act=0;
            }
        }
        
        Net::poll();
        
        if(!cs->is_connected())
            break;
    }
    cs->close();
}
