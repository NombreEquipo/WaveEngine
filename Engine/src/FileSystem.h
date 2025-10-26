#pragma once

#include "Module.h"
#include <iostream>
#include <vector>
#include <string>

class GameObject;
struct aiNode;
struct aiScene;

struct Mesh {
	unsigned int num_vertices = 0;
	float* vertices = nullptr;

	unsigned int num_indices = 0;
	unsigned int* indices = nullptr;

	float* texCoords = nullptr;  

	unsigned int id_vertex = 0;   // VBO
	unsigned int id_index = 0;    // EBO
	unsigned int id_texcoord = 0; 
	unsigned int id_VAO = 0;
};

class FileSystem : public Module
{
public:
	FileSystem();
	~FileSystem();

	bool Awake() override;
	bool Start() override;
	bool Update() override;
	bool CleanUp() override;

	// Load a specific FBX file
	bool LoadFBX(const std::string& file_path, unsigned int flag = 0);

	// Load FBX from assets folder (relative path)
	bool LoadFBXFromAssets(const std::string& filename);

	// Add a mesh to the list
	void AddMesh(const Mesh& mesh);

	// Get all loaded meshes
	const std::vector<Mesh>& GetMeshes() const { return meshes; }

	// Clear all meshes
	void ClearMeshes();
	
	// Load FBX and convert to GameObject 
	GameObject* LoadFBXAsGameObject(const std::string& file_path);


private:

	GameObject* ProcessNode(aiNode* node, const aiScene* scene);

	int CountNodes(aiNode* node);

	std::vector<Mesh> meshes;
	std::string assetsPath;

};