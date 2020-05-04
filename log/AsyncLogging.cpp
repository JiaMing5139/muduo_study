//
// Created by parallels on 4/14/20.
//

#include "AsyncLogging.h"
#include <iostream>
#include <assert.h>
#include <chrono>
#include "appendFile.h"
#include "debug.h"
AsyncLogging::AsyncLogging(std::string name):
mutex_(),
cond_(),
currentBuffer_(new Buffer),
nextBuffer_(new Buffer),
buffersptr_(new Buffers)
{
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffersptr_->reserve(16);

}

void AsyncLogging::append(const char * msg, size_t len) {
    // atomic operation
    mutex_.lock();
   // std:: cout << "main thread:current avail:"<<currentBuffer_->avail() <<" len:" << len << std::endl;
    if(currentBuffer_->avail() >= len){
      //  debug("main thread:add to current buffer")
        currentBuffer_->append(msg,len);
            // append to currentbuffer
    }else{
        assert(currentBuffer_->avail() < len);
        // buffer has been full
        buffersptr_->push_back(std::move(currentBuffer_));
        if(nextBuffer_){
            currentBuffer_ = std::move(nextBuffer_);
        }else{
            currentBuffer_.reset(new Buffer);
            //allocate new memory for nextbuffer
        }
    //    std:: cout << "main thread:current notify"<< std::endl;
        cond_.notify_one();
    }
    mutex_.unlock();

}

void AsyncLogging::threadFunc() {
    appendFile output("asynglogtest");
    Bufferptr Buffer1(new Buffer);
    Bufferptr Buffer2(new Buffer);
    std::unique_ptr<Buffers> newbuffers(new Buffers);
    char buffertest[] ="testconetent\n";
    output.append(buffertest,sizeof buffertest - 1);

    while(this->isRunning_){
        assert(Buffer1 && Buffer1->length() == 0);
        assert(Buffer2 && Buffer2->length() == 0);
        assert(newbuffers->empty());

     {
     //    std::cout << "thread wait mutex" << std::endl;
                std::unique_lock<std::mutex> lock(mutex_);
                if(buffersptr_->empty()){
                    cond_.wait_for(lock,std::chrono::seconds(3));
                }

                buffersptr_->push_back(std::move(currentBuffer_));
                currentBuffer_ = std::move(Buffer1);
                std::swap(newbuffers,buffersptr_);
                if(!nextBuffer_){
                    nextBuffer_ = std::move(Buffer2);
            }
         }
        //output
        assert(newbuffers->size()<16);
        for( auto & writeBuffer: *newbuffers){
    //        std::cout << "Asynthread:write buffer length:" << writeBuffer->length() << std::endl;
           output.append(writeBuffer->begin(),writeBuffer->length());
        }

        if(!Buffer1){
            assert(!newbuffers->empty());
            Buffer1 = std::move(newbuffers->back());
            newbuffers->pop_back();
            Buffer1->reset();
        }




        if(!Buffer2){
            assert(!newbuffers->empty());
            Buffer2 = std::move(newbuffers->back());
            newbuffers->pop_back();
            Buffer2->reset();
        }

        newbuffers->clear();
        output.flush();
        }
    output.flush();

}
