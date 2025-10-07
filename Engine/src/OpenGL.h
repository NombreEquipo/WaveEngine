#pragma once
#include "Module.h"
#include <SDL3/SDL_video.h>  

class OpenGL : public Module
{
public:
    OpenGL();
    ~OpenGL();

    bool Start() override;
    bool Update() override;
    bool CleanUp() override;

private:
    SDL_GLContext glContext;
    unsigned int shaderProgram;
    unsigned int VAO;
    unsigned int VBO;
};