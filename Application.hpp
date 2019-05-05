#pragma once

class Application
{
  public:
    inline static Application& instance() noexcept
    {
        static Application app;
        return app;
    }

    inline static void init() noexcept { instance(); }


  private:
    Application() noexcept;
    ~Application();
};