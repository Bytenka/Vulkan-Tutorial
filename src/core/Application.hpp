#pragma once
#include "pch.hpp"

class Application
{
  public:
    static void run();

  private:
    Application();
    ~Application();
    Application(const Application&) = delete;
    void operator=(const Application&) = delete;
};