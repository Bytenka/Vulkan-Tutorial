#include "pch.hpp"

#include "Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <stdexcept>

Logger::Logger()
{
    try {
        m_console = spdlog::stdout_color_mt("console");
        spdlog::set_default_logger(m_console);

        m_console->set_level(spdlog::level::info);
        m_console->set_pattern("[%T:%e] <%^%l%$> %v");

        // Do not use the LOG_* macros in the contructor
        m_console->trace("Initialized logger");

    } catch (const spdlog::spdlog_ex& ex) {
        throw std::runtime_error("Failed to initialize logger\n" + std::string(ex.what()));
    }
}


Logger::~Logger()
{
    m_console->trace("Destroying logger");
}

// public:

void Logger::setLoggingLevel(Logger::LogLevel level) noexcept
{
    try {
        switch (level) {
        case LOG_TRACE: m_console->set_level(spdlog::level::trace); break;
        case LOG_DEBUG: m_console->set_level(spdlog::level::debug); break;
        case LOG_INFO: m_console->set_level(spdlog::level::info); break;
        case LOG_WARN: m_console->set_level(spdlog::level::warn); break;
        case LOG_ERROR: m_console->set_level(spdlog::level::err); break;
        case LOG_CRITICAL: m_console->set_level(spdlog::level::critical); break;
        default: break;
        }

    } catch (const spdlog::spdlog_ex& ex) {
        LOG_ERROR("Could not set logging level to {}\n{}", level, ex.what());
    }
}
