#include "Application.h"
#include <iostream>

Application::Application() : isRunning(true)
{
    std::cout << "Application Constructor" << std::endl;
    window = std::make_shared<Window>();
}

Application& Application::GetInstance()
{
    static Application instance;
    return instance;
}

bool Application::Init()
{
    std::cout << "Application Init" << std::endl;

    if (!window->Init())
    {
        std::cerr << "Failed to initialize Window!" << std::endl;
        return false;
    }

    return true;
}

bool Application::Update()
{
    // Update window (handles events)
    if (!window->Update())
    {
        isRunning = false;
    }

    // Clear screen to black and present
    window->Render();

    return isRunning;
}

bool Application::CleanUp()
{
    std::cout << "Application CleanUp" << std::endl;

    if (window)
    {
        window->CleanUp();
    }

    return true;
}



// para cuando tengamos mas modulos
//void Engine::AddModule(std::shared_ptr<Module> module) {
//    module->Init();
//    moduleList.push_back(module);
//}
// llamar en el constructor a esta funcion y añadir por ej.     AddModule(std::static_pointer_cast<Module>(window));
// luego para todas las cosas donde tengamos que recorrer modulos (ej. update?) usamos esa lista