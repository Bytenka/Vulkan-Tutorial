#include "pch.hpp"

#include "Window.hpp"
#include "core/Exception.hpp"

#include <GLFW/glfw3.h>

Window::Window(int width, int height, const std::string& title)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (!m_glfwWindow) {
        throw Exception("Could not create window " + title);
    }

    glfwSetWindowUserPointer(m_glfwWindow, this); // To get the Window object from the GLFW pointer

    glfwMakeContextCurrent(nullptr);
}


Window::~Window()
{
    glfwDestroyWindow(m_glfwWindow);
}

// public

void const Window::update() noexcept {
}


bool const Window::shouldClose() noexcept {
    return glfwWindowShouldClose(m_glfwWindow);
}