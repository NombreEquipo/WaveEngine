#pragma once

#include <memory>
#include <list>
#include "Window.h"
#include "Module.h"
#include "Input.h"
#include "OpenGL.h"
#include "FileSystem.h"

class Module;

class Application
{
public:
    // Singleton instance
    static Application& GetInstance();

    // Called before render is available
    bool Awake();

    // Called before the first frame
    bool Start();

    // Called each loop iteration
    bool Update();

    // Called before quitting
    bool CleanUp();

    // Modules
    std::shared_ptr<Window> window;
    std::shared_ptr<Input> input;
    std::shared_ptr<OpenGL> opengl;
    std::shared_ptr<FileSystem> filesystem;

private:
    // Private constructor for singleton
    Application();
    ~Application() = default;

    // Delete copy constructor and assignment
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void AddModule(std::shared_ptr<Module> module);
    std::list<std::shared_ptr<Module>> moduleList;

    bool isRunning;

    // Call modules before each loop iteration
    bool PreUpdate();

    // Call modules on each loop iteration
    bool DoUpdate();

    // Call modules after each loop iteration
    bool PostUpdate();

public:

    enum EngineState
    {
        CREATE = 1,
        AWAKE,
        START,
        LOOP,
        CLEAN,
        FAIL,
        EXIT
    };

};