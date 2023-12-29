#include "Log.h"
#include "File.h"
#include "Time.h"
#include "Configuration.h"

#include <iostream>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <unistd.h>

#if defined(LOG_DLT)
#include <dlt/dlt.h>
DLT_DECLARE_CONTEXT(mainContext);
#endif

#define LOG_APPID "FSW"

Log* Log::getInstance()
{
    static Log *pLog = nullptr;

    if(pLog == nullptr)
        pLog = new Log();

    return pLog;
}

void Log::registerService(ILogger::Ptr logger) noexcept
{
    mLoggers.push_back(logger);

#if defined(LOG_DLT)
    DLT_REGISTER_APP(LOG_APPID, "Factory Software");
    DLT_REGISTER_CONTEXT(mainContext, LOG_APPID, "Factory Software context");
#endif
}

void Log::out(LogLevel lv, const char* format, ...) noexcept
{
    char buf[BUF_SIZE];
    va_list args;

    va_start(args, format);
    vsnprintf(buf, BUF_SIZE, format, args);
    va_end(args);

#if defined(LOG_FILE)
    std::stringstream ss("");
    std::string tag = "";
    
    if (lv == LogLevel::Verbose)    tag = "[V]";
    else if (lv == LogLevel::Debug) tag = "[D]";
    else if (lv == LogLevel::Info)  tag = "[I]";
    else if (lv == LogLevel::Warn)  tag = "[W]";
    else if (lv == LogLevel::Error) tag = "[E]";
    else                            tag = "[F]";

    ss << "[" << utils::time::now() << "]" << "[" << std::to_string(getpid()) << "]" << tag << buf << '\n';
    utils::file::write(LOG_DIRECTORY, ss.str().c_str());
#endif

#if defined(LOG_DLT)
    mLoggers[0]->out(lv, buf); //ADB Logcat
#endif

}

void Log::out(LogLevel lv, const char* fileName, const unsigned long line, const char* func, const char* text, const uint8_t buf[], unsigned int size) noexcept
{
    std::stringstream ss("");

    ss << "[" << fileName << ":" << line << "][" << func << "]" << text;

    for (unsigned int i=0; i<size; i++)
        ss << " " << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(buf[i]);
#if defined(LOG_FILE)
    ss << '\n';
    utils::file::write(LOG_DIRECTORY, ss.str().c_str());
#endif

#if defined(LOG_DLT)
    mLoggers[0]->out(lv, ss.str().c_str()); //ADB Logcat
#endif

}

void Logger::out(LogLevel lv, const char* str) noexcept
{
    if (lv == LogLevel::Verbose)
#if defined(LOG_DLT)
        DLT_LOG(mainContext, DLT_LOG_VERBOSE, DLT_STRING(str));
#else
        std::cout << "[V]" << str << std::endl;
#endif
    else if (lv == LogLevel::Debug)
#if defined(LOG_DLT)
        DLT_LOG(mainContext, DLT_LOG_DEBUG, DLT_STRING(str));
#else
        std::cout << "[D]" << str << std::endl;
#endif
    else if (lv == LogLevel::Info)
#if defined(LOG_DLT)
        DLT_LOG(mainContext, DLT_LOG_INFO, DLT_STRING(str));
#else
        std::cout << "[I]" << str << std::endl;
#endif
    else if (lv == LogLevel::Warn)
#if defined(LOG_DLT)
        DLT_LOG(mainContext, DLT_LOG_WARN, DLT_STRING(str));
#else
        std::cout << "[W]" << str << std::endl;
#endif
    else if (lv == LogLevel::Error)
#if defined(LOG_DLT)
        DLT_LOG(mainContext, DLT_LOG_ERROR, DLT_STRING(str));
#else
        std::cout << "[E]" << str << std::endl;
#endif
    else if (lv == LogLevel::Fatal)
#if defined(LOG_DLT)
        DLT_LOG(mainContext, DLT_LOG_FATAL, DLT_STRING(str));
#else
        std::cout << "[F]" << str << std::endl;
#endif
}
