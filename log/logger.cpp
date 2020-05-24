//
// Created by parallels on 4/9/20.
//

#include "logger.h"
#include <iostream>
#include <time.h>
#include <sstream>
#include <string.h>
#include "../base/currentThread.h"
namespace Jimmy {
Logger::LogLevel initLogLevel(){
    //fixme from a config or env
    return Logger::TRACE;
}
__thread char t_errnobuf[512];

void defaultoutput(const char * msg,size_t len){
    std::cout << msg ;
}

Logger::Output Logger::output = defaultoutput;

Logger::LogLevel  g_logLevel =  initLogLevel();

void Jimmy::Logger::setLevel(Jimmy::Logger::LogLevel level) {
    g_logLevel = level;
}


    Logger::Logger(const std::string & file, int line, Logger::LogLevel level, const char *func,bool ifabort):
    ifabort(ifabort)
    {
        time_t seconds = ::time(nullptr);
       auto locoaltimeStruct = localtime(&seconds);
        std::string time_s(asctime(locoaltimeStruct));
      //  std::string time_s ="None";
       // time_s.pop_back();
        std:: string log_type;
        switch (level){
            case TRACE: log_type = "TRACE";
                break;
            case ERROR:log_type = "ERROR";
                break;
        }
        _os<<"tid:" << Jimmy::CurrentThread::tid() << " " << time_s <<" "<< file <<" line:" << line << " " <<  log_type <<" ";
    }






    Logger::~Logger() {
        if(errno >0 ){
#ifdef __linux__
            strcpy(t_errnobuf,strerror_r(errno,t_errnobuf,sizeof t_errnobuf));
#endif
#ifdef __APPLE__
            strerror_r(errno,t_errnobuf,sizeof t_errnobuf);
#endif
            _os <<" errno:" << errno <<" ";
           _os << t_errnobuf;
        }
        _os << std::endl;
        Logger::output(_os.str().c_str(),_os.str().size());


        if(ifabort){

            abort();
        }

    }

    void Logger::setOutput(Output out) {
        Logger::output = std::move(out);
    }



}