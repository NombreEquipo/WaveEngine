#include "Application.h"
#include <iostream>

int main(int argc, char* argv[])
{
    LOG_CONSOLE("Starting Application...");

    Application& app = Application::GetInstance();

    // Awake
    if (!app.Awake())
    {
        LOG_CONSOLE("Failed to awake application!");
        return -1;
    }

    // Start
    if (!app.Start())
    {
        LOG_CONSOLE("Failed to start application!");
        return -1;
    }

    // Main loop
    while (app.Update())
    {
        // Application running
    }

    // Clean up
    app.CleanUp();

    LOG_CONSOLE("Application closed successfully");

    return 0;
}