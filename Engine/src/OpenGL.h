#pragma once
#include "Module.h"
#include <SDL3/SDL.h>
struct SDL_Window;

class OpenGL : public Module
{
public:
	OpenGL();
	~OpenGL();

	SDL_GLContext glContext;
	unsigned int shaderProgram;
	unsigned int VAO;
	unsigned int VBO;

private:
	bool Start() override;
	bool CleanUp() override;
	bool Update() override;
};