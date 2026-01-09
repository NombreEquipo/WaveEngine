#pragma once
#include "Module.h"
#include <memory>
#include <vector>
#include <lua.hpp>

class GameObject;
class FileSystem;
class Renderer;
class ComponentCamera;
class SceneWindow;

class ModuleScripting
{
public:
    ModuleScripting();
    virtual ~ModuleScripting();

    virtual bool Start();
    virtual bool Update();
    virtual bool CleanUp();

    bool LoadScript(const char* path);
    void PushInput();
    glm::vec3 MouseWorldPos();
    //int Lua_SetPosition(lua_State* L);

    GameObject* owner = NULL;

    std::string filePath;
    std::string name;

    lua_State* GetLuaState() { return L; }
private:
    lua_State* L;
    bool init = true;
};
