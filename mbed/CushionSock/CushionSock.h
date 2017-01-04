/*
* CushionSock.h
* Created by 3116igbt
* Original source code is https://github.com/3116igbt/16hacku_zabuton
*
* CushionSock.h is set of wrapper functions of Websocket.h
*/

#ifndef CUSHION_SOCK_H
#define CUSHION_SOCK_H

#include "mbed.h"
#include "Websocket.h"
#include "EthernetNetIf.h"
#include "MbedJSONValue.h"


class CushionSock
{
    public:
    int MESS_STOP;
    int MESS_START;
    int MESS_SIT;
    int MESS_FRUST;
    int MESS_TEST;
    int MESS_TWEET;
    
    /*
        Constructor
        @param hostAndport format : "hostname:port"        
    */
    CushionSock();  
    
    void init(char * hostAndPort);
    /*
       connect to server
       @return true if the connection is established, false otherwise
    */
    bool connect();
    
    /*
       send message to server
       @return the number of bytes sent 
       @param type Message type(prease show and choose from line 14 to 17 in souece code)
       @param msg message payload  
    */    
    int mess_send(int type, char * msg);
    
    /*
        Recieve message from server
        @return if recieved succsessful, Message type(prease show and choose from line 14 to 17 in souece code), otherwise returns -1.
        @param msg pointer to the message to be read.
    */
    int mess_recv(char * msg);
    
    /*
     To see if there is a SocketIO connection active
     @return true if there is a connection active
    */
    bool is_connected();
    
    bool close();
    
    private:
    EthernetNetIf *eth;
    Websocket *ws;
};
#endif