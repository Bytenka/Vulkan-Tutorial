#pragma once
#include "pch.hpp"

#include "VulkanCore.hpp"
#include "graphics/Window.hpp"

#include <map>
#include <memory>

class Application
{
  public:
    static void run(int argc, const char* argv[]);

  private:
    static void signalHandler(int signum) noexcept;
    static void errorCallbackGLFW(int error, const char* description);
    static bool processCommandLineArgs(int argc, const char* argv[]) noexcept;
    static void stdoutUsage() noexcept;

    void createWindow(int width, int height, const std::string& title);
    void destroyWindow(WindowID id);
    void destroyAllWindows() noexcept;


  private:
    bool m_shouldStop;

    WindowID m_currentWindowID = 1;
    std::map<WindowID, std::unique_ptr<Window>> m_windows;

    std::unique_ptr<VulkanCore> m_vulkanCore;

  private:
    static Application* s_instance;
    Application();
    ~Application();

  public:
    Application(const Application&) = delete;
    void operator=(const Application&) = delete;
};