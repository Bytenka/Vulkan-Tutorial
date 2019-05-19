#pragma once
#include "pch.hpp"

#include "VulkanCore.hpp"
#include "graphics/Window.hpp"

#include <map>
#include <memory>

#define NO_MAIN_WINDOW 0
#define FIRST_WINDOW_ID 1
typedef unsigned long long WindowID;

class Application
{
  public:
    static void run(int argc, const char* argv[]);

  private:
    static void signalHandler(int signum) noexcept;
    static void errorCallbackGLFW(int error, const char* description);
    static bool processCommandLineArgs(int argc, const char* argv[]) noexcept;
    static void stdoutUsage() noexcept;

    WindowID createWindow(int width, int height, const std::string& title);
    void destroyWindow(WindowID id);
    void destroyAllWindows() noexcept;


  private:
    bool m_shouldStop;

    WindowID m_currentWindowID = FIRST_WINDOW_ID;
    WindowID m_mainWindowID = NO_MAIN_WINDOW;
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