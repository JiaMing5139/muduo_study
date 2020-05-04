//
// Created by parallels on 4/9/20.
//

#include "logger.h"
#include <iostream>
#include <time.h>
#include <sstream>
#include "../base/currentThread.h"
namespace Jimmy {
Logger::LogLevel initLogLevel(){
    //fixme from a config or env
    return Logger::DEBUG;
}

void defaultoutput(const char * msg,size_t len){
    std::cout << msg << std::endl;
}

Logger::Output Logger::output = defaultoutput;

Logger::LogLevel  g_logLevel =  initLogLevel();

void Jimmy::Logger::setLevel(Jimmy::Logger::LogLevel level) {
    g_logLevel = level;
}


    Logger::Logger(const std::string & file, int line, Logger::LogLevel level, const char *func) {
        time_t seconds = ::time(nullptr);
        auto locoaltimeStruct = localtime(&seconds);
        std::string time_s(asctime(locoaltimeStruct));
        time_s.pop_back();
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
        _os << std::endl;
        Logger::output(_os.str().c_str(),_os.str().size());
    }

    void Logger::setOutput(Output out) {
        Logger::output = std::move(out);
    }


}