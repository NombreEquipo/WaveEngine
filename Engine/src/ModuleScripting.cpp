#include "ModuleScripting.h"
#include "Renderer.h"
#include "FileSystem.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "Transform.h"
#include <float.h>
#include <functional>
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include <nlohmann/json.hpp>
#include <fstream>

GameObject* own = NULL;

ModuleScripting::ModuleScripting()
{
}

ModuleScripting::~ModuleScripting()
{
}
int  Lua_SetPosition(lua_State* L)
{
    GameObject* go = (GameObject*)lua_touserdata(L, 1);
    if (go == NULL)return 0;

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    float z = luaL_checknumber(L, 4);

    Transform* transform = static_cast<Transform*>(go->GetComponent(ComponentType::TRANSFORM));
    transform->SetPosition({ x, y, z });

    return 0;
}
int  Lua_SetRotation(lua_State* L)
{
    GameObject* go = (GameObject*)lua_touserdata(L, 1);
    if (go == NULL)return 0;
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    float z = luaL_checknumber(L, 4);

    Transform* transform = static_cast<Transform*>(go->GetComponent(ComponentType::TRANSFORM));
    transform->SetRotation({ x, y, z });

    return 0;
}
void GetAllGameObjects(GameObject* root, std::vector<GameObject*>& outObjects)
{
    if (root == nullptr)
        return;

    outObjects.push_back(root);

    const std::vector<GameObject*>& children = root->GetChildren();
    for (GameObject* child : children)
    {
        GetAllGameObjects(child, outObjects);
    }
}
int Lua_FindGameObject(lua_State* L)
{
    std::string name = luaL_checkstring(L, 1);
    GameObject* go = NULL;

    if (name != "this")
    {
        std::vector<GameObject*> objects;
        GetAllGameObjects(Application::GetInstance().scene.get()->GetRoot(), objects);

        for (auto& obj : objects)
            if (obj->GetName() == name) go = obj;

        if (!go)
        {
            lua_pushnil(L);
            return 1;
        }
    }
    else go = own;

    lua_pushlightuserdata(L, go);
    return 1;
}

bool ModuleScripting::Start()
{
    LOG_DEBUG("Initializing ModuleScripting");
    own = owner;
    L = luaL_newstate();
    luaL_openlibs(L);
    lua_register(L, "SetPosition", Lua_SetPosition);
    lua_register(L, "SetRotation", Lua_SetRotation);
    lua_register(L, "FindGameObject", Lua_FindGameObject);


    LOG_CONSOLE("ModuleScripting ready");

    return true;
}

bool ModuleScripting::Update()
{
    if (init) {
        
        //LoadScript("../Library/Scripts/test.lua");
        lua_getglobal(L, "Start");
        if (lua_isfunction(L, -1))
            lua_pcall(L, 0, 0, 0);

        init = false;
    }

    lua_getglobal(L, "Update");
    if (lua_isfunction(L, -1))
        lua_pcall(L, 0, 0, 0);

    return true;
}
bool ModuleScripting::LoadScript(const char* path)
{
    if (luaL_loadfile(L, path) != LUA_OK)
    {
        LOG_CONSOLE("Lua load error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }

    if (lua_pcall(L, 0, 0, 0) != LUA_OK)
    {
        LOG_CONSOLE("Lua runtime error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }
    filePath = path;
    name = filePath.substr( filePath.find_last_of("/\\")+1);
    return true;
}

bool ModuleScripting::CleanUp()
{
    lua_close(L);
    LOG_DEBUG("Cleaning up ModuleScripting");
    return true;
}