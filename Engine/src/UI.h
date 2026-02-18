#pragma once
#include "Module.h"
#include "Octree.h"
#include <memory>
#include <vector>

class GameObject;
class FileSystem;
class Renderer;
class ComponentCamera;
class SceneWindow;

class UI : public Module
{
public:
    UI();
    ~UI();

    bool Start() override;
    bool Update() override;
    bool CleanUp() override;
    bool PreUpdate() override;

private:

};