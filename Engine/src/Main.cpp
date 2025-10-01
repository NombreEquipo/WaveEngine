#include "Application.h"
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Starting Application..." << std::endl;

    Application& app = Application::GetInstance();

    // Initialize
    if (!app.Init())
    {
        std::cerr << "Failed to initialize application!" << std::endl;
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