#include "pch.hpp"

#include "Application.hpp"
#include "Exception.hpp"
#include "Logger.hpp"
#include "VulkanCore.hpp"
#include "graphics/Window.hpp"

#include <csignal>
#include <iostream>

// Initializing static members
Application* Application::s_instance = nullptr;


Application::Application()
    : m_shouldStop(false)
{
    try {
        if (Application::s_instance)
            throw Exception("An instance of Application already exists. Only one instance is allowed");

        // Initialize the logger singleton (if it was not initialized already)
        // by calling this once
        Logger::instance();

        if (glfwInit() == GLFW_FALSE)
            throw Exception("Failed to initialize GLFW");

        glfwSetErrorCallback(Application::errorCallbackGLFW);

        // Place signal handlers
        std::signal(SIGINT, Application::signalHandler);


        m_vulkanCore = std::make_unique<VulkanCore>();

        // Register this instance
        Application::s_instance = this;

        LOG_TRACE("Initialized Application");

    } catch (const std::runtime_error& ex) {
        throw Exception("Failed to initialize Application\n" + std::string(ex.what()));
    }
}


Application::~Application()
{
    LOG_TRACE("Closing Application");

    // Make sure everything has been cleared before calling glfwTerminate
    if (m_windows.size() > 0) {
        LOG_TRACE("Destroying remaining Windows ({})", m_windows.size());
        destroyAllWindows();
    }

    glfwTerminate();

    // Reset signal handlers
    std::signal(SIGINT, SIG_DFL);

    Application::s_instance = nullptr;
}

// public:

void Application::run(int argc, const char* argv[])
{
    // Exit if command line args are invalid
    if (!Application::processCommandLineArgs(argc, argv)) return;

    Application app;
    app.m_mainWindowID = app.createWindow(654, 498, "Test");
    app.createWindow(456, 723, "Test2");

    while (!app.m_shouldStop) {
        std::vector<WindowID> windowsToDestroy;

        glfwPollEvents();

        for (auto& w : app.m_windows) {
            auto& currentWindow = w.second;


            if (currentWindow->shouldClose()) {
                windowsToDestroy.push_back(w.first);

                if (w.first == app.m_mainWindowID) {
                    LOG_TRACE("Main Window resquested closing, terminating Application.");
                    app.m_shouldStop = true;
                }
            }

            currentWindow->update();
        }

        // Destroy everything here to prevent iterator invalidation
        for (auto& w : windowsToDestroy)
            app.destroyWindow(w);
    }

    app.destroyAllWindows();
}

// private

void Application::signalHandler(int signum) noexcept
{
    switch (signum) {
    case SIGINT:
        Application::s_instance->m_shouldStop = true;
        break;

    default:
        std::cout << "Unhandled signal received: " << signum << std::endl;
        break;
    }
}


void Application::errorCallbackGLFW(int error, const char* description)
{
    LOG_ERROR("GLFW error: {}", description);
}


bool Application::processCommandLineArgs(int argc, const char* argv[]) noexcept
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
                    return false;
                }

                Logger::instance().setLoggingLevel(static_cast<Logger::LogLevel>(lvl));  // This cast is safe
                ++i;

            } catch (const std::exception& ex) {
                stdoutUsage();
                return false;
            }
        } else {
            stdoutUsage();
            return false;
        }
        ++i;
    }

    return true;
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


WindowID Application::createWindow(int width, int height, const std::string& title)
{
    WindowID cacheID = m_currentWindowID;
    auto newWindow = std::make_unique<Window>(width, height, title);

    m_windows.insert({m_currentWindowID++, std::move(newWindow)});

    LOG_TRACE("Added Window \"{}\" to handler (WindowID: {})", title, cacheID);
    return cacheID;
}


void Application::destroyWindow(WindowID id)
{
    auto it = m_windows.find(id);
    if (it == m_windows.end()) {
        LOG_WARN("Trying to destroy an invalid Window");
        LOG_WARN("Window with ID {} does not exist");

    } else {
        LOG_TRACE("Removing Window \"{}\" from handler (WindowID: {})", it->second->getTitle(), it->first);
        m_windows.erase(it);
    }
}


void Application::destroyAllWindows() noexcept
{
    LOG_TRACE("Clearing Window handler");
    m_windows.clear();
    m_currentWindowID = FIRST_WINDOW_ID;
}