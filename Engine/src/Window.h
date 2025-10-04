#pragma once

#include <SDL3/SDL.h>
#include "Module.h"

class Window : public Module
{
public:
    Window();
    ~Window();

    bool Start() override; 

    // Handle events (returns false if quit requested)
    bool Update() override;

    // Clear screen and present
    void Render();

    bool PostUpdate() override;

    // Clean up resources
    bool CleanUp() override;

    // Getters
    void GetWindowSize(int& width, int& height) const;
    int GetScale() const;

    SDL_Window* GetWindow() const { return window; }

private:
    SDL_Window* window;
    //SDL_Renderer* renderer;

    int width;
    int height;
    int scale;
};