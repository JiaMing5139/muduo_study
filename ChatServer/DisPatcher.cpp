//
// Created by parallels on 5/27/20.
//

#include "DisPatcher.h"
void defaultOnMessage(DisPatcher::Messageptr msg){
    std::cout<< "defaultCallback:" << std::endl <<msg->DebugString() << std::endl;
}

void DisPatcher::onMessage(const DisPatcher::Messageptr msg) {
    OnProtoMessageCallback cb = callbacks_[msg->GetDescriptor()->name()];
    if(cb){
        cb(msg);
    }else{
        defaultOnMessage(msg);
    }

}

void DisPatcher::registerCallback(std::string type, DisPatcher::OnProtoMessageCallback cb) {
    callbacks_[type] = cb;
}


