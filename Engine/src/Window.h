#pragma once

#include <SDL3/SDL.h>

class Window
{
public:
    Window();
    ~Window();

    // Initialize window and renderer
    bool Init();

    // Handle events (returns false if quit requested)
    bool Update();

    // Clear screen and present
    void Render();

    // Clean up resources
    bool CleanUp();

    // Getters
    void GetWindowSize(int& width, int& height) const;
    int GetScale() const;

private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    int width;
    int height;
    int scale;
};