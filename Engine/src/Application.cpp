#include "Application.h"
#include <iostream>

Application::Application() : isRunning(true)
{
    std::cout << "Application Constructor" << std::endl;
    window = std::make_shared<Window>();
    input = std::make_shared<Input>();
    opengl = std::make_shared<OpenGL>();
    filesystem = std::make_shared<FileSystem>();

    AddModule(std::static_pointer_cast<Module>(window));
    AddModule(std::static_pointer_cast<Input>(input));
    AddModule(std::static_pointer_cast<OpenGL>(opengl));
    AddModule(std::static_pointer_cast<FileSystem>(filesystem));
}

Application& Application::GetInstance()
{
    static Application instance;
    return instance;
}

void Application::AddModule(std::shared_ptr<Module> module)
{
    moduleList.push_back(module);
}

bool Application::Awake() 
{
    return true;
}

bool Application::Start()
{
    bool result = true;
    for (const auto& module : moduleList) {
        result = module.get()->Start();
        if (!result) {
            break;
        }
    }

    return true;
}

bool Application::Update()
{
    bool ret = true;

    if (input->GetWindowEvent(WE_QUIT) == true)
        ret = false;

    if (ret == true)
        ret = PreUpdate();

    if (ret == true)
        ret = DoUpdate();

    if (ret == true)
        ret = PostUpdate();

    return ret;
}

bool Application::PreUpdate() 
{
    //Iterates the module list and calls PreUpdate on each module
    bool result = true;
    for (const auto& module : moduleList) {
        result = module.get()->PreUpdate();
        if (!result) {
            break;
        }
    }

    return result;
}

// Call modules on each loop iteration
bool Application::DoUpdate()
{
    //Iterates the module list and calls Update on each module
    bool result = true;
    for (const auto& module : moduleList) {
        result = module.get()->Update();
        if (!result) {
            break;
        }
    }

    return result;
}

// Call modules on each loop iteration
bool Application::PostUpdate()
{
    //Iterates the module list and calls Update on each module
    bool result = true;
    for (const auto& module : moduleList) {
        result = module.get()->PostUpdate();
        if (!result) {
            break;
        }
    }

    return result;
}

bool Application::CleanUp()
{
    std::cout << "Application CleanUp" << std::endl;

    bool result = true;
    for (const auto& module : moduleList) {
        result = module.get()->CleanUp();
        if (!result) {
            break;
        }
    }

    return result;
}
