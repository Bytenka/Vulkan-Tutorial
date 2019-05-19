#pragma once

#include <GLFW/glfw3.h>

#include <string>

class Window
{
  public:
    Window() : Window(640, 480){};
    explicit Window(int width, int height) : Window(width, height, "Tuto"){};
    explicit Window(int width, int height, const std::string& title);
    ~Window();

    void update() const noexcept;
    bool shouldClose() noexcept;
    inline const std::string& getTitle() const noexcept { return m_title; }

  private:
    GLFWwindow* m_glfwWindow = nullptr;
    std::string m_title;

  public:
    Window(const Window&) = delete;
    void operator=(const Window&) = delete;
};