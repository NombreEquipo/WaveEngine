#pragma once

#include <memory>
#include "Window.h"

class Application
{
public:
    // Singleton instance
    static Application& GetInstance();

    // Main application flow
    bool Init();
    bool Update();
    bool CleanUp();

    // Modules
    std::shared_ptr<Window> window;

private:
    // Private constructor for singleton
    Application();
    ~Application() = default;

    // Delete copy constructor and assignment
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    bool isRunning;
};