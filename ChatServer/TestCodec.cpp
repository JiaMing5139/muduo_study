//
// Created by parallels on 5/26/20.
//
#include "Codec.h"
#include "ChatMessage.pb.h"
#include "DisPatcher.h"
#include <iostream>
typedef struct {
    int32_t  len;
    int32_t  namelen;
}MessageHeader;


using namespace std;
int main(){
    Jimmy::MessageChat request;
    request.set_sequence(1);
    string s("hello,world1231231231111113");
    request.set_datapayload(std::move(s));

    Codec c;
    char buff[8192] = {0};
    int len ;
    c.FillBuffer(request,buff,&len);
   // memcpy(buff,(void *) s.c_str(), s.size());
    Codec::ErrorCode code;
    Codec::MessagePtr msg =    c.parse(buff + 4,len, &code);
    cout << code << endl;
    DisPatcher dis;
    std::shared_ptr<Jimmy::MessageChat> mss =   static_pointer_cast<Jimmy::MessageChat>(msg);

    dis.registerCallback(Jimmy::MessageChat::GetDescriptor()->name(), [](Codec::MessagePtr msg){
        cout << "Request " << msg->DebugString() << endl;
        std::shared_ptr<Jimmy::MessageChat> mss =   static_pointer_cast<Jimmy::MessageChat>(msg); // not good
        std::cout << mss->datapayload() << endl;
    });

    dis.onMessage(mss);



}
