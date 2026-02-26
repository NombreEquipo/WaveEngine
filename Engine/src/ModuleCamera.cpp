#include "ModuleCamera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "Input.h"
#include "Window.h"

ModuleCamera::ModuleCamera() :
    Module()
{
    name = "Camera";
    mainCamera = nullptr;
}

ModuleCamera::~ModuleCamera()
{

}

void ModuleCamera::AddCamera(ComponentCamera* cam) {
    
    cameras.push_back(cam);
}

void ModuleCamera::RemoveCamera(ComponentCamera* cam)
{
    auto it = std::find(cameras.begin(), cameras.end(), cam);

    if (it != cameras.end())
    {
        cameras.erase(it);
        LOG_DEBUG("Camera removed from ModuleCamera");
    }
}

bool ModuleCamera::CleanUp()
{
    LOG_DEBUG("Cleaning up Camera Module");

    cameras.clear();
    mainCamera = nullptr;

    return true;
}

void ModuleCamera::SetMainCamera(ComponentCamera* camera)
{
    mainCamera = camera;
}
