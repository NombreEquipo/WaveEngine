#pragma once
#include "Module.h"
#include <SDL3/SDL_video.h>  
#include <iostream>

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
    unsigned int CreatePyramid();


private:

    SDL_GLContext glContext;
    unsigned int shaderProgram;
    Uint VAO_Triangle, VAO_Cube, VAO_Pyramid, VAO_Cylinder;
    Uint VBO, EBO;
    // EBO = Element buffer object

};