//
// Created by parallels on 5/27/20.
//

#ifndef CHAT_DISPATCHER_H
#define CHAT_DISPATCHER_H

#include <map>
#include <memory>
#include <google/protobuf/message.h>
class DisPatcher {
public:
    typedef std::shared_ptr<google::protobuf::Message> Messageptr;
    typedef std::function<void (Messageptr)> OnProtoMessageCallback;

    void onMessage(Messageptr);
    void registerCallback(std::string type,OnProtoMessageCallback);

private:
    typedef std::map<std::string,OnProtoMessageCallback> Callbacks;
    Callbacks  callbacks_;
};


#endif //CHAT_DISPATCHER_H
