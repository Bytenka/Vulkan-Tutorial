#pragma once
#include "pch.hpp"

class Application
{
  public:
    static void run(int argc, const char* argv[]);

  private:
    static void signalHandler(int signum) noexcept;

    void processCommandLineArgs(int argc, const char* argv[]);
    void stdoutUsage() noexcept;

  private:
    bool m_shouldStop;

  private:
    static Application* instance;
    Application();
    ~Application();
    Application(const Application&) = delete;
    void operator=(const Application&) = delete;
};