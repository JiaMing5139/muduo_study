//
// Created by parallels on 5/27/20.
//

#include "InetAddress.h"
#include "ChatClient.h"
#include <iostream>
int main(int argc,char ** argv){
    if(argc != 3){
        std::cerr << "usage: ./ChatClient ip port" << std::endl;
        abort();
    }

    InetAddress addr(argv[1],atoi(argv[2]));
    ChatClient chatClient(addr);
    chatClient.start();
}