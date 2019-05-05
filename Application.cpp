#include "pch.hpp"
#include "Application.hpp"

Application::Application() noexcept
{
    if (glfwInit() == GLFW_FALSE) {
        // Error
    }

}

Application::~Application()
{
    glfwTerminate();
}