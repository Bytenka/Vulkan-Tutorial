#include "pch.hpp"

#include "Application.hpp"
#include "Logger.hpp"

#include <csignal>
#include <iostream>

// Initializing static members
Application* Application::instance = nullptr;


Application::Application()
    : m_shouldStop(false)
{
    try {
        if (Application::instance)
            throw Exception("An instance of Application already exists. Only one instance is allowed");

        // Initialize the logger singleton (if it was not initialized already)
        // by calling this once
        Logger::instance();

        if (glfwInit() == GLFW_FALSE)
            throw Exception("Failed to initialize GLFW");


        // Place signal handlers
        std::signal(SIGINT, Application::signalHandler);


        // Register this instance
        Application::instance = this;

    } catch (const std::runtime_error& ex) {
        throw Exception("Failed to initialize Application: " + std::string(ex.what()));
    }
}

Application::~Application()
{
    glfwTerminate();

    // Reset signal handlers
    std::signal(SIGINT, SIG_DFL);


    Application::instance = nullptr;
}

// public:

void Application::run(int argc, const char* argv[])
{
    Application app;
    app.processCommandLineArgs(argc, argv);
    LOG_TRACE("Initialized Application");

    while (!app.m_shouldStop) {
    }

    LOG_TRACE("Application terminating");
}

// private

void Application::signalHandler(int signum) noexcept
{
    switch (signum) {
    case SIGINT:
        Application::instance->m_shouldStop = true;
        break;

    default:
        std::cout << "Unhandled signal received: " << signum << std::endl;
        break;
    }
}

void Application::processCommandLineArgs(int argc, const char* argv[])
{
    int i = 1;
    while (i < argc) {
        if (!std::strcmp(argv[i], "-d") || !std::strcmp(argv[i], "--debug")) {
            Logger::instance().setLoggingLevel(Logger::LogLevel::LOG_DEBUG);

        } else if (!std::strcmp(argv[i], "--debug-level")) {
            // @see Logger::LogLevel for reference
            try {
                int lvl = std::stoi(argv[i + 1]);  // Convertion errors are catched below
                if (lvl < Logger::LogLevel::LOG_MIN || lvl > Logger::LogLevel::LOG_MAX) {
                    stdoutUsage();
                    m_shouldStop = true;
                    return;
                }

                Logger::instance().setLoggingLevel(static_cast<Logger::LogLevel>(lvl));  // This cast is safe
                ++i;

            } catch (const std::exception& ex) {
                stdoutUsage();
                m_shouldStop = true;
                return;
            }
        }
        ++i;
    }
}

void Application::stdoutUsage() noexcept
{
    std::cout << "Usage:" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "  -d, --debug             Starts the program in debug mode" << std::endl
              << "  --debug-level LEVEL     Set the logging level. ([0-5], none=0, default=2)" << std::endl

              << std::endl;
}
