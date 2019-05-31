#include "pch.hpp"

#include "Window.hpp"
#include "core/Application.hpp"
#include "core/Exception.hpp"
#include "core/Logger.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

Window::Window(int width, int height, const std::string& title)
    : m_title(title)
{
    try {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

        if (!m_glfwWindow)
            throw Exception("GLFW failed to create window \"" + title + "\"");

        // Create a surface to draw to
        if (glfwCreateWindowSurface(Application::getVulkanInstance()->get(), m_glfwWindow, nullptr, &m_surface) != VK_SUCCESS)
            throw Exception("Failed to create window surface");

        glfwSetWindowUserPointer(m_glfwWindow, this);  // To get the Window object from the GLFW pointer

        LOG_TRACE("Initialized Window \"{}\" ({}, {})", title, width, height);

    } catch (const Exception& ex) {
        LOG_ERROR("Failed to initialize Window \"{}\"", title);
        throw;
    }
}


Window::~Window()
{
    LOG_TRACE("Destroying Window \"{}\"", m_title);
    vkDestroySurfaceKHR(Application::getVulkanInstance()->get(), m_surface, nullptr);
    glfwDestroyWindow(m_glfwWindow);
}

// public

void Window::update() const
{
}


bool Window::shouldClose() noexcept
{
    return glfwWindowShouldClose(m_glfwWindow);
}