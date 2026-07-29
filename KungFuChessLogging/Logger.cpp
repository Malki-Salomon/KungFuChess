#include "Logger.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

namespace Logger
{
    namespace
    {
        std::mutex g_mutex;
        std::ofstream g_file;
        bool g_initialized = false;

        const char* levelName(Level level)
        {
            switch (level)
            {
                case Level::Info:  return "INFO";
                case Level::Warn:  return "WARN";
                case Level::Error: return "ERROR";
            }
            return "INFO";
        }

        std::string timestamp()
        {
            auto now = std::chrono::system_clock::now();
            std::time_t nowAsTime = std::chrono::system_clock::to_time_t(now);
            std::tm localTime{};
#ifdef _WIN32
            localtime_s(&localTime, &nowAsTime);
#else
            localtime_r(&nowAsTime, &localTime);
#endif
            std::ostringstream out;
            out << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
            return out.str();
        }
    }

    void init(const std::string& logFilePath)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_initialized)
            return;

        g_file.open(logFilePath, std::ios::out | std::ios::app);
        g_initialized = true;
    }

    void log(Level level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(g_mutex);

        std::string line = "[" + timestamp() + "] [" + levelName(level) + "] " + message;

        std::cout << line << "\n";

        // Flushed on every write rather than left buffered - this is a
        // debugging/persistence tool ("figure out what went wrong"), so
        // a line surviving a crash matters more than the extra I/O cost
        // at this project's scale.
        if (g_file.is_open())
        {
            g_file << line << "\n";
            g_file.flush();
        }
    }

    void info(const std::string& message)
    {
        log(Level::Info, message);
    }

    void warn(const std::string& message)
    {
        log(Level::Warn, message);
    }

    void error(const std::string& message)
    {
        log(Level::Error, message);
    }
}
