#pragma once
#include "Module.h"
#include "ComponentCamera.h"
#include <memory>
#include <vector>

class GameObject;

class ModuleCamera : public Module
{
public:
    ModuleCamera();
    ~ModuleCamera();

    bool CleanUp() override;

    ComponentCamera* GetMainCamera() const { return mainCamera; }
    void SetMainCamera(ComponentCamera*);

    void AddCamera(ComponentCamera* camera);
    void RemoveCamera(ComponentCamera* camera);

private:  
    std::vector<ComponentCamera*> cameras;
    ComponentCamera* mainCamera;     
};

