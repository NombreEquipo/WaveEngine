#pragma once
#include "Module.h"
#include "GameObject.h"

class FileSystem;
class Renderer;

class ModuleScene : public Module
{
public:
    ModuleScene();
    virtual ~ModuleScene();

    bool Awake() override;
    bool Start() override;
    bool Update() override;
    bool CleanUp() override;

    GameObject* CreateGameObject(const std::string& name);

    GameObject* GetRoot() const { return root; }

private:

    GameObject* root = nullptr;
	Renderer* renderer = nullptr;
	FileSystem* filesystem = nullptr;
};

