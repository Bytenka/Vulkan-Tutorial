#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <string>

class VulkanInstance;
class Window
{
  public:
    Window() : Window(640, 480){};
    explicit Window(int width, int height) : Window(width, height, "Tuto"){};
    explicit Window(int width, int height, const std::string& title);
    ~Window();

    void update() const;
    bool shouldClose() noexcept;
    inline const std::string& getTitle() const noexcept { return m_title; }

  private:
    GLFWwindow* m_glfwWindow = nullptr;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    std::string m_title;

  public:
    Window(const Window&) = delete;
    void operator=(const Window&) = delete;
};