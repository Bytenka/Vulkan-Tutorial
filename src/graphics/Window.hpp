#pragma once

#include <GLFW/glfw3.h>

#include <string>

typedef unsigned long long WindowID;

class Window
{
  public:
    Window() : Window(640, 480){};
    Window(int width, int height) : Window(width, height, "Tuto"){};
    Window(int width, int height, const std::string& title);
    ~Window();

    void const update() noexcept;
    bool const shouldClose() noexcept;

  private:
    GLFWwindow* m_glfwWindow = nullptr;

  public:
    Window(const Window&) = delete;
    void operator=(const Window&) = delete;
};