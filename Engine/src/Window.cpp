#include "Window.h"
#include <iostream>

Window::Window() : window(nullptr), renderer(nullptr), width(1280), height(720), scale(1)
{
    std::cout << "Window Constructor" << std::endl;
}

Window::~Window()
{
}

bool Window::Init()
{
    std::cout << "Init SDL3 Window & Renderer" << std::endl;

    // Initialize SDL3
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create window
    window = SDL_CreateWindow(
        "SDL3 Window",
        width,
        height,
        SDL_WINDOW_RESIZABLE
    );

    if (window == nullptr)
    {
        std::cerr << "Window creation failed! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, nullptr);

    if (renderer == nullptr)
    {
        std::cerr << "Renderer creation failed! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Window and Renderer created successfully" << std::endl;
    return true;
}

bool Window::Update()
{
    SDL_Event event;

    // Poll events
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            return false;
        }

        // Handle window close button
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        {
            return false;
        }
    }

    return true;
}

void Window::Render()
{
    // Clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Present the rendered frame
    SDL_RenderPresent(renderer);
}

bool Window::CleanUp()
{
    std::cout << "Destroying SDL Window and Renderer" << std::endl;

    if (renderer != nullptr)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
    return true;
}

void Window::GetWindowSize(int& width, int& height) const
{
    width = this->width;
    height = this->height;
}

int Window::GetScale() const
{
    return scale;
}