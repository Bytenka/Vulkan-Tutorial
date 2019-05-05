#include "pch.hpp"

#include "Application.hpp"
#include "Logger.hpp"

Application::Application()
{
    try {
        // Initialize the logger singleton by calling this once
        Logger::instance();

        if (glfwInit() == GLFW_FALSE)
            throw Exception("Failed to initialize GLFW");
        LOG_TRACE("Initialized GLFW");

    } catch (const std::runtime_error& ex) {
        throw Exception("Failed to initialize Application: " + std::string(ex.what()));
    }
}

Application::~Application()
{
    LOG_TRACE("Application closing");
    glfwTerminate();
}

// public:

void Application::run()
{
    Application app;

    while (false) {
    }
}