/*
* CushionSock.cpp
* Created by 3116igbt
* Original source code is https://github.com/3116igbt/16hacku_zabuton
*
* CushionSock.cpp is imprementation of CushionSock.h.
*/
#include "CushionSock.h"
#define STOP 0x01
#define START 0x02
#define SIT 0x11
#define FRUST 0x12
#define TEST 0x21
#define TWEET 0x13

CushionSock::CushionSock(){
    printf("begin\r\n");

    MESS_STOP = 0x01;
    MESS_START = 0x02;
    MESS_SIT = 0x11;
    MESS_FRUST = 0x12;
    MESS_TEST =0x21;
    MESS_TWEET = 0x13;
}

void CushionSock::init(char * hostAndPort){
    eth = new EthernetNetIf();
    EthernetErr ethErr = eth->setup();
    if (ethErr) {
        printf("\r\nERROR %d in setup.\r\n", ethErr);
    }
    printf("hostandport %s\n", hostAndPort);
    
    ws = new Websocket(hostAndPort, eth);
    printf("websock init\n");
}
    
bool CushionSock::connect(){
    printf("connecting...");
    int failcount=0;
    
    while(! ws->connect()) {
        failcount++;
        if(failcount>10){
            printf("cannot connect failed.\n");
            return false;
        }
        printf("cannot connect websocket, retrying...\n");
        wait(2);
    }
     return true;
}
  
int CushionSock::mess_send(int type, char * msg){
    string name;
    switch(type){
        case TEST:
            name="test";
            break;
        case STOP:
            name="stop";
            break;
        case START:
            name="start";
            break;
        case SIT:
            name="status_sit";
            break;
        case FRUST:
            name="status_frust";
            break;
        case TWEET:
            name="status_tweet";
        default:
            name="null";
    }
    MbedJSONValue json;
    json["name"]=name.c_str();
    json["value"]=msg;
    ws->send((char*)json.serialize().c_str());
    printf("data send\n");
    return 1;
} 
    
int CushionSock::mess_recv(char * msg){
    if (ws->read(msg)) {
        int type=0;
        printf("recieved msg : %s \n",msg);
        if(strcmp(msg,"start")==0)
            type=MESS_START;
        if(strcmp(msg,"stop")==0)
            type=MESS_STOP;
        return type;
    }else{
        return -1;
    }
}
 
bool CushionSock::is_connected(){
    return ws->connected();
}

bool CushionSock::close(){
    return ws->close();
}