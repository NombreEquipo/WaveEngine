#include "FileSystem.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h> 
#include <assimp/cimport.h>
#include "OpenGL.h"
#include <iostream>

FileSystem::FileSystem() : Module()
{

}

FileSystem:: ~FileSystem() 
{

}

bool FileSystem::Awake()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	return true;
}

bool FileSystem::Start() 
{

	std::string file_path = "C:\\Users\\anaam7\\Documents\\GitHub\\Engine\\Engine\\warrior.FBX";
	LoadFBX(file_path, 1);

	return true;
}

bool FileSystem::Update()
{
	return true;
}

void FileSystem::LoadFBX(std::string& file_path, unsigned int flag)
{
	const aiScene* scene = aiImportFile("C:\\Users\\anaam7\\Documents\\GitHub\\Engine\\Engine\\warrior.FBX", aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		aiReleaseImport(scene);
	}
	else
		std::cout << "Error loading scene" << std::endl;

}

bool FileSystem::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

