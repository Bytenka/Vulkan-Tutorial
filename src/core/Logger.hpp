#pragma once
#include "pch.hpp"

#include <spdlog/spdlog.h>

#include <memory>

class Logger
{
  public:
    enum LogLevel {
        TRACE = 0,
        INFO,
        DEBUG,
        WARN,
        ERROR,
        CRITICAL
    };

    inline static Logger& instance()
    {
        static Logger logger;
        return logger;
    }

    inline const std::shared_ptr<spdlog::logger>& logger() const noexcept { return m_console; }

  private:
    std::shared_ptr<spdlog::logger> m_console = nullptr;

    private:
      Logger();
      ~Logger();
      Logger(const Logger&) = delete;
      void operator=(const Logger&) = delete;
};

#define LOG_TRACE(message, args...) Logger::instance().logger()->trace(message, ##args)
#define LOG_INFO(message, args...) Logger::instance().logger()->info(message, ##args)
#define LOG_DEBUG(message, args...) Logger::instance().logger()->debug(message, ##args)
#define LOG_WARN(message, args...) Logger::instance().logger()->warn(message, ##args)
#define LOG_ERROR(message, args...) Logger::instance().logger()->error(message, ##args)
#define LOG_CRITICAL(message, args...) Logger::instance().logger()->critical(message, ##args)