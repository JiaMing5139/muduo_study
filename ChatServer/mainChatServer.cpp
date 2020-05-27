//
// Created by parallels on 5/27/20.
//
#include "InetAddress.h"
#include "ChatServer.h"
int main(){
    InetAddress addr(2333);
    ChatServer chatServer(addr);
    chatServer.start();
}