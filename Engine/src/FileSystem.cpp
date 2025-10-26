#include "FileSystem.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h> 
#include <assimp/cimport.h>
#include <windows.h>
#include "Application.h"
#include "GameObject.h"
#include "Transform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

FileSystem::FileSystem() : Module() {}
FileSystem::~FileSystem() {}

bool FileSystem::Awake()
{
	return true;
}

bool FileSystem::Start()
{
	// Get directory of executable
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string execPath(buffer);
	size_t pos = execPath.find_last_of("\\/");
	std::string execDir = execPath.substr(0, pos);

	// Move up two levels: from build/ to Engine/, then to root
	pos = execDir.find_last_of("\\/");
	std::string parentDir = execDir.substr(0, pos);
	pos = parentDir.find_last_of("\\/");
	std::string rootDir = parentDir.substr(0, pos);

	std::string housePath = rootDir + "\\Assets\\BakerHouse.fbx";

	std::cout << "Trying to load: " << housePath << std::endl;

	GameObject* houseModel = LoadFBXAsGameObject(housePath);

	if (houseModel != nullptr)
	{
		GameObject* root = Application::GetInstance().scene->GetRoot();
		root->AddChild(houseModel);
		std::cout << "FBX loaded" << std::endl;
	}
	else
	{
		GameObject* pyramidObject = new GameObject("Pyramid");
		ComponentMesh* meshComp = static_cast<ComponentMesh*>(pyramidObject->CreateComponent(ComponentType::MESH));
		Mesh pyramidMesh = Primitives::CreatePyramid();
		meshComp->SetMesh(pyramidMesh);

		GameObject* root = Application::GetInstance().scene->GetRoot();
		root->AddChild(pyramidObject);

		std::cerr << "Failed to load FBX. Use drag & drop." << std::endl;
	}
	// For loading Baker_house at the beggining
	return true;
}

bool FileSystem::Update()
{
	if (Application::GetInstance().input->HasDroppedFile())
	{
		std::string filePath = Application::GetInstance().input->GetDroppedFilePath();
		DroppedFileType fileType = Application::GetInstance().input->GetDroppedFileType();
		Application::GetInstance().input->ClearDroppedFile();

		if (fileType == DROPPED_FBX)
		{
			GameObject* loadedModel = LoadFBXAsGameObject(filePath);
			if (loadedModel != nullptr)
			{
				GameObject* root = Application::GetInstance().scene->GetRoot();
				root->AddChild(loadedModel);

				std::cout << "Model loaded" << std::endl;
				std::cout << "   Root GameObject: " << loadedModel->GetName() << std::endl;
				std::cout << "   Children: " << loadedModel->GetChildren().size() << std::endl;
			}
			else
			{
				std::cerr << "Failed to load FBX" << std::endl;
			}

			// Clean up previous meshes before loading new ones
			//ClearMeshes();

			//if (LoadFBX(filePath))
			//{
			//	std::cout << "Successfully loaded FBX file!" << std::endl;
			//}
			//else
			//{
			//	std::cerr << "Failed to load FBX file!" << std::endl;
			//}
		}
		else if (fileType == DROPPED_TEXTURE)
		{
			// Apply texture to the current mesh on screen (change it when we have more than one mesh in screen)
			Application::GetInstance().renderer->LoadTexture(filePath);
		}
	}

	return true;
}

bool FileSystem::LoadFBX(const std::string& file_path, unsigned int flag)
{
	std::cout << "Loading FBX: " << file_path << std::endl;

	// Flags with coordinate conversion
	unsigned int importFlags = flag != 0 ? flag :
		aiProcessPreset_TargetRealtime_MaxQuality |
		aiProcess_ConvertToLeftHanded;

	const aiScene* scene = aiImportFile(file_path.c_str(), importFlags);

	if (scene == nullptr)
	{
		std::cerr << "Error loading: " << aiGetErrorString() << std::endl;
		return false;
	}

	if (!scene->HasMeshes())
	{
		std::cerr << "No meshes in scene" << std::endl;
		aiReleaseImport(scene);
		return false;
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* aiMesh = scene->mMeshes[i];
		Mesh mesh;

		mesh.num_vertices = aiMesh->mNumVertices;
		mesh.vertices = new float[mesh.num_vertices * 3];

		// Copy vertices 
		float scale = 0.05f;
		for (unsigned int v = 0; v < mesh.num_vertices; ++v)
		{
			mesh.vertices[v * 3] = aiMesh->mVertices[v].x;
			mesh.vertices[v * 3 + 1] = aiMesh->mVertices[v].y;
			mesh.vertices[v * 3 + 2] = aiMesh->mVertices[v].z;
		}

		if (aiMesh->HasTextureCoords(0))
		{
			mesh.texCoords = new float[mesh.num_vertices * 2];
			for (unsigned int v = 0; v < mesh.num_vertices; ++v)
			{
				mesh.texCoords[v * 2] = aiMesh->mTextureCoords[0][v].x;
				mesh.texCoords[v * 2 + 1] = aiMesh->mTextureCoords[0][v].y;
			}
		}
		else
		{
			// If there are no UVs, create default coordinates
			mesh.texCoords = new float[mesh.num_vertices * 2];
			for (unsigned int v = 0; v < mesh.num_vertices * 2; ++v)
			{
				mesh.texCoords[v * 2] = 0.0f;
				mesh.texCoords[v * 2 + 1] = 0.0f;
			}
		}

		// Indices
		if (aiMesh->HasFaces())
		{
			mesh.num_indices = aiMesh->mNumFaces * 3;
			mesh.indices = new unsigned int[mesh.num_indices];

			for (unsigned int j = 0; j < aiMesh->mNumFaces; ++j)
				memcpy(&mesh.indices[j * 3], aiMesh->mFaces[j].mIndices, 3 * sizeof(unsigned int));
		}

		Application::GetInstance().renderer->LoadMesh(mesh);
		meshes.push_back(mesh);

		std::cout << "Mesh " << i << ": " << mesh.num_vertices << " vertices, " << mesh.num_indices << " indices" << std::endl;
	}

	aiReleaseImport(scene);
	return true;
}

bool FileSystem::LoadFBXFromAssets(const std::string& filename)
{
	return LoadFBX("Assets/" + filename, aiProcessPreset_TargetRealtime_MaxQuality);
}

void FileSystem::AddMesh(const Mesh& mesh)
{
	meshes.push_back(mesh);
}

void FileSystem::ClearMeshes()
{
	for (auto& mesh : meshes)
	{
		Application::GetInstance().renderer->UnloadMesh(mesh);
		delete[] mesh.vertices;
		delete[] mesh.indices;
		delete[] mesh.texCoords;
	}
	meshes.clear();
	std::cout << "All meshes cleared" << std::endl;
}

bool FileSystem::CleanUp()
{
	ClearMeshes();
	aiDetachAllLogStreams();
	return true;
}

GameObject* FileSystem::LoadFBXAsGameObject(const std::string& file_path)
{
	LOG("Loading FBX as GameObject: %s", file_path.c_str());

	unsigned int importFlags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded;

	const aiScene* scene = aiImportFile(file_path.c_str(), importFlags);

	if (scene == nullptr)
	{
		LOG("Corrupt archive");
		return nullptr;
	}

	if (!scene->HasMeshes())
	{
		LOG("No meshes");
		aiReleaseImport(scene);
		return nullptr;
	}

	std::cout << "==== FBX loaded ====" << std::endl;
	std::cout << "  Meshes: " << scene->mNumMeshes << std::endl;
	std::cout << "  Materials: " << scene->mNumMaterials << std::endl;
	std::cout << "  Nodes: " << CountNodes(scene->mRootNode) << std::endl;

	GameObject* rootObject = ProcessNode(scene->mRootNode, scene);

	aiReleaseImport(scene);

	LOG("GameObject created");
	return rootObject;
}

int FileSystem::CountNodes(aiNode* node)
{
	int count = 1;
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		count += CountNodes(node->mChildren[i]);
	}
	return count;
}

GameObject* FileSystem::ProcessNode(aiNode* node, const aiScene* scene)
{
	// ======================================================== 1 ====================================================
	//Create GameObject 

	std::string nodeName = node->mName.C_Str();
	if (nodeName.empty()) nodeName = "Unnamed";

	GameObject* gameObject = new GameObject(nodeName);

	LOG("Processing node: %s", nodeName.c_str());

	// ======================================================== 2 ====================================================

	//Get Transform component
	Transform* transform = static_cast<Transform*>(gameObject->GetComponent(ComponentType::TRANSFORM));

	if (transform != nullptr)
	{
		// Get transformation from aiNode
		aiVector3D position, scaling;
		aiQuaternion rotation;
		node->mTransformation.Decompose(scaling, rotation, position);

		// Set transform
		float scale = 0.01f; 
		transform->SetPosition(glm::vec3(position.x, position.y, position.z));
		transform->SetScale(glm::vec3(scaling.x, scaling.y, scaling.z));
		transform->SetRotationQuat(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));

		LOG("  Transform - Pos: (%.2f, %.2f, %.2f), Scale: (%.2f, %.2f, %.2f)", position.x, position.y, position.z, scaling.x, scaling.y, scaling.z);
	}

	// ======================================================== 3 ====================================================
	// Process meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		unsigned int meshIndex = node->mMeshes[i];
		aiMesh* aiMesh = scene->mMeshes[meshIndex];

		LOG("  Processing mesh %d: %s", i, aiMesh->mName.C_Str());

		ComponentMesh* meshComponent = static_cast<ComponentMesh*>(gameObject->CreateComponent(ComponentType::MESH));

		Mesh mesh;
		mesh.num_vertices = aiMesh->mNumVertices;
		mesh.vertices = new float[mesh.num_vertices * 3];

		// Copy vertexes
		for (unsigned int v = 0; v < mesh.num_vertices; v++)
		{
			mesh.vertices[v * 3 + 0] = aiMesh->mVertices[v].x;
			mesh.vertices[v * 3 + 1] = aiMesh->mVertices[v].y;
			mesh.vertices[v * 3 + 2] = aiMesh->mVertices[v].z;
		}

		// Copy texture coordinates
		if (aiMesh->HasTextureCoords(0))
		{
			mesh.texCoords = new float[mesh.num_vertices * 2];
			for (unsigned int v = 0; v < mesh.num_vertices; v++)
			{
				mesh.texCoords[v * 2 + 0] = aiMesh->mTextureCoords[0][v].x;
				mesh.texCoords[v * 2 + 1] = aiMesh->mTextureCoords[0][v].y;
			}
			LOG("    UV coordinates: YES");
		}
		else
		{
			mesh.texCoords = new float[mesh.num_vertices * 2];
			for (unsigned int v = 0; v < mesh.num_vertices * 2; v++)
			{
				mesh.texCoords[v] = 0.0f;
			}
			LOG("    UV coordinates: NO");
		}

		// Copy indexes
		if (aiMesh->HasFaces())
		{
			mesh.num_indices = aiMesh->mNumFaces * 3;
			mesh.indices = new unsigned int[mesh.num_indices];

			for (unsigned int f = 0; f < aiMesh->mNumFaces; f++)
			{
				memcpy(&mesh.indices[f * 3], aiMesh->mFaces[f].mIndices, 3 * sizeof(unsigned int));
			}
		}
		
		LOG("    Vertices: %d, Indices: %d", mesh.num_vertices, mesh.num_indices);

		meshComponent->SetMesh(mesh);

		delete[] mesh.vertices;
		delete[] mesh.indices;
		delete[] mesh.texCoords;

		// ======================================================== 4 ====================================================

		// Material 
		if (aiMesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];

			// Has base color?
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString texturePath;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

				LOG("    Material has texture: %s", texturePath.C_Str());

				// Create ComponentMaterial
				ComponentMaterial* matComponent = static_cast<ComponentMaterial*>(gameObject->GetComponent(ComponentType::MATERIAL));
				
				//If we don't have one
				if (matComponent == nullptr)
				{
					matComponent = static_cast<ComponentMaterial*>(gameObject->CreateComponent(ComponentType::MATERIAL));
				}

				// Assign texture to material
				std::string textureFile = texturePath.C_Str();

				if (textureFile.find('/') == std::string::npos && textureFile.find('\\') == std::string::npos)
				{
					textureFile = "Assets\\" + textureFile;
				}

				matComponent->LoadTexture(textureFile);
			}
		}
	}

	//========================================================= 5 ====================================================

	// Recursive for children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		GameObject* child = ProcessNode(node->mChildren[i], scene);
		if (child != nullptr)
		{
			gameObject->AddChild(child);
		}
	}

	return gameObject;
}

