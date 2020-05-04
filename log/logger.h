//
// Created by parallels on 4/9/20.
//

#ifndef LOG_LOGGER_H
#define LOG_LOGGER_H

#include <string>
#include <list>
#include <memory>
#include <sstream>
#include <ostream>

namespace Jimmy {
    class Logger{
    public:
        typedef std::function<void (const  char*,size_t)> Output;
        Logger(const Logger &) = delete;
        Logger& operator= (const Logger &) = delete;
        enum LogLevel
        {
            TRACE,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            NUM_LOG_LEVELS,
        };

        Logger(const std::string &file, int line, LogLevel level, const char* func);
        ~Logger();

#define LOG_TRACE  if(Jimmy::Logger::TRACE <= Jimmy::Logger::logLevel()) \
         Jimmy::Logger(__FILE__, __LINE__, Jimmy::Logger::TRACE, __func__).stream() \

#define SYSERROR_LOG Jimmy::Logger(__FILE__, __LINE__, Jimmy::Logger::ERROR, __func__).stream()


        std::ostream & stream(){
            return this->_os;
        }

        static LogLevel logLevel();
        static void  setLevel(LogLevel level);
        static void setOutput(Output out);
        static Output output;

    private:


        std::ostringstream _os;
        LogLevel _loglevel;
        int _line;

    };
    extern  Logger::LogLevel  g_logLevel;
    inline Logger::LogLevel Logger::logLevel()
    {
        return g_logLevel;
    }



}


#endif //LOG_LOGGER_H
