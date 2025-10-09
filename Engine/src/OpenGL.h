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

    unsigned int CreateTriangle();
    unsigned int CreateCube(); 

private:

    typedef unsigned int Uint, uint;

    SDL_GLContext glContext;
    unsigned int shaderProgram;
    Uint VAO_Triangle, VAO_Cube;
    Uint VBO, EBO;
    // EBO = Element buffer object

};