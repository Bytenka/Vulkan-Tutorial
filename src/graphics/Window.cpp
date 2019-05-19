#include "pch.hpp"

#include "Window.hpp"
#include "core/Exception.hpp"
#include "core/Logger.hpp"

#include <GLFW/glfw3.h>

Window::Window(int width, int height, const std::string& title)
    : m_title(title)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (!m_glfwWindow) {
        throw Exception("GLFW failed to create window \"" + title + "\"");
    }

    glfwSetWindowUserPointer(m_glfwWindow, this);  // To get the Window object from the GLFW pointer

    glfwMakeContextCurrent(nullptr);

    LOG_TRACE("Initialized Window \"{}\" ({}, {})", title, width, height);
}


Window::~Window()
{
    glfwDestroyWindow(m_glfwWindow);
    LOG_TRACE("Destroyed Window \"{}\"", m_title);
}

// public

void Window::update() const noexcept
{
}


bool Window::shouldClose() noexcept
{
    return glfwWindowShouldClose(m_glfwWindow);
}