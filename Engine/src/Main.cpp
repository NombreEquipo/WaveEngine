#include "Application.h"
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Starting Application..." << std::endl;

    Application& app = Application::GetInstance();

    // Awake
    if (!app.Awake())
    {
        std::cerr << "Failed to awake application!" << std::endl;
        return -1;
    }

    // Start
    if (!app.Start())
    {
        std::cerr << "Failed to start application!" << std::endl;
        return -1;
    }

    // Main loop
    while (app.Update())
    {
        // Application running
    }

    // Clean up
    app.CleanUp();

    std::cout << "Application closed successfully" << std::endl;

    return 0;
}