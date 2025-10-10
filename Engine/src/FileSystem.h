#pragma once

#include "Module.h"

class FileSystem : public Module
{
public:
	FileSystem();
	~FileSystem();

	bool Awake() override;

	// Called before the first frame
	bool Start() override;

	// Called each loop iteration
	bool Update() override;

	// Called before quitting
	bool CleanUp() override;

private:
	void LoadFBX(std::string& file_path, unsigned int flag);

	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;
	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

};

