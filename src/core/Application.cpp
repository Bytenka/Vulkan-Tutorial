#include "pch.hpp"

#include "Application.hpp"
#include "Exception.hpp"
#include "Logger.hpp"
#include "graphics/Window.hpp"
#include "vulkan/VulkanInstance.hpp"

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

        m_vulkanInstance = std::make_unique<VulkanInstance>();

        // Register this instance
        Application::s_instance = this;

        // Place signal handlers
        std::signal(SIGINT, Application::signalHandler);

        LOG_TRACE("Initialized Application");

    } catch (const std::runtime_error& ex) {
        // Here we can't log the error, because the logger may not be initialized
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
    try {
        // Exit if command line args are invalid
        if (!Application::processCommandLineArgs(argc, argv)) return;
        Application app;

        app.m_mainWindowID = app.createWindow(654, 498, "Test");
        app.createWindow(456, 723, "Test2");


        while (!app.m_shouldStop) {
            try {
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

            } catch (const Exception& ex) {
                if (ex.isFatal())
                    throw;
                else
                    LOG_ERROR("Exception caught: {}", ex.what());
            }
        }

        app.destroyAllWindows();

    } catch (const Exception& ex) {
        // At this point, unhandeled exceptions are concidered fatal.
        // Resources are freed, Application destructor was called
        LOG_CRITICAL("Fatal error occured: {}", ex.what());
        throw;
    }
}


// private

void Application::signalHandler(int signum) noexcept
{
    switch (signum) {
    case SIGINT:
        LOG_TRACE("Interrupt signal received");
        Application::s_instance->m_shouldStop = true;
        break;

    default:
        LOG_WARN("Unhandled signal received: {}", signum);
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
              << "  --debug-level LEVEL     Set the logging level. ([0-5], none=0, default=3)" << std::endl

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