#include "pch.hpp"

#include "Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <memory>
#include <stdarg.h>
#include <stdexcept>

Logger::Logger()
{
    try {
        m_console = spdlog::stdout_color_mt("console");
        spdlog::set_default_logger(m_console);

        m_console->set_level(spdlog::level::trace);

        // Do not use the LOG_* macros in the contructor
        m_console->info("Initialized logger");

    } catch (const spdlog::spdlog_ex& ex) {
        throw std::runtime_error("Failed to initialize logger: " + std::string(ex.what()));
    }
}

Logger::~Logger()
{
}

// public:
