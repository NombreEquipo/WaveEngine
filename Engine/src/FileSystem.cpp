#include "FileSystem.h"
#define NOMINMAX  
#include <assimp/scene.h>
#include <assimp/postprocess.h> 
#include <assimp/cimport.h>
#include <windows.h>
#include <algorithm>
#include <limits>
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
    // Get executable directory and move to Assets folder
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string execPath(buffer);

    size_t pos = execPath.find_last_of("\\/");
    std::string execDir = execPath.substr(0, pos);

    // Go up two levels: build/ -> Engine/ -> root/
    pos = execDir.find_last_of("\\/");
    std::string parentDir = execDir.substr(0, pos);
    pos = parentDir.find_last_of("\\/");
    std::string rootDir = parentDir.substr(0, pos);

    std::string housePath = rootDir + "\\Assets\\BakerHouse.fbx";

    std::cout << "Loading: " << housePath << std::endl;

    GameObject* houseModel = LoadFBXAsGameObject(housePath);

    if (houseModel != nullptr)
    {
        GameObject* root = Application::GetInstance().scene->GetRoot();
        root->AddChild(houseModel);
        std::cout << "FBX loaded successfully" << std::endl;
    }
    else
    {
        // Fallback: create a pyramid primitive if FBX fails
        GameObject* pyramidObject = new GameObject("Pyramid");
        ComponentMesh* meshComp = static_cast<ComponentMesh*>(pyramidObject->CreateComponent(ComponentType::MESH));
        Mesh pyramidMesh = Primitives::CreatePyramid();
        meshComp->SetMesh(pyramidMesh);

        GameObject* root = Application::GetInstance().scene->GetRoot();
        root->AddChild(pyramidObject);

        std::cerr << "Failed to load FBX." << std::endl;
    }

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

                LOG( "Model loaded: %s", loadedModel->GetName().c_str());
                LOG( "Children count: %zu", loadedModel->GetChildren().size());
            }
            else
            {
                LOG("✗ Failed to load FBX");
            }
        }
        else if (fileType == DROPPED_TEXTURE)
        {
            // Get all selected objects in the editor
            std::vector<GameObject*> selectedObjects =
                Application::GetInstance().selectionManager->GetSelectedObjects();

            if (!selectedObjects.empty())
            {
                // Apply the texture to all selected objects
                int successCount = 0;
                for (GameObject* obj : selectedObjects)
                {
                    if (ApplyTextureToGameObject(obj, filePath))
                    {
                        successCount++;
                    }
                }

                LOG( "✓ Texture applied to %d of %zu selected objects",
                    successCount, selectedObjects.size());
            }
            else
            {
                // Fallback behavior: load the texture globally if no objects are selected
                LOG( "No objects selected. Loading texture globally.");
                Application::GetInstance().renderer->LoadTexture(filePath);
            }
        }
    }

    return true;
}

bool FileSystem::CleanUp()
{
    aiDetachAllLogStreams();
    return true;
}

bool FileSystem::ApplyTextureToGameObject(GameObject* obj, const std::string& texturePath)
{
    if (!obj || !obj->IsActive())
        return false;

    bool applied = false;

    // Check if the object has a mesh
    ComponentMesh* meshComp = static_cast<ComponentMesh*>(obj->GetComponent(ComponentType::MESH));

    if (meshComp && meshComp->IsActive() && meshComp->HasMesh())
    {
        // Get or create the material component
        ComponentMaterial* matComp = static_cast<ComponentMaterial*>(
            obj->GetComponent(ComponentType::MATERIAL));

        if (matComp == nullptr)
        {
            matComp = static_cast<ComponentMaterial*>(
                obj->CreateComponent(ComponentType::MATERIAL));
        }

        // Load the texture
        if (matComp->LoadTexture(texturePath))
        {
            LOG(__FILE__, __LINE__, "✓ Texture applied to: %s", obj->GetName().c_str());
            applied = true;
        }
        else
        {
            LOG(__FILE__, __LINE__, "✗ Failed to apply texture to: %s", obj->GetName().c_str());
        }
    }

    // Recursively apply to children
    for (GameObject* child : obj->GetChildren())
    {
        if (ApplyTextureToGameObject(child, texturePath))
        {
            applied = true;
        }
    }

    return applied;
}


GameObject* FileSystem::LoadFBXAsGameObject(const std::string& file_path)
{
    LOG("Loading FBX: %s", file_path.c_str());

    unsigned int importFlags =
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices |
        aiProcess_OptimizeMeshes |
        aiProcess_ValidateDataStructure;

    const aiScene* scene = aiImportFile(file_path.c_str(), importFlags);

    if (scene == nullptr)
    {
        LOG("Import failed: %s", aiGetErrorString());
        return nullptr;
    }

    if (!scene->HasMeshes())
    {
        LOG("No meshes found in scene");
        aiReleaseImport(scene);
        return nullptr;
    }

    std::string directory = file_path.substr(0, file_path.find_last_of("/\\"));

    std::cout << "==== FBX Loaded ====" << std::endl;
    std::cout << "Meshes: " << scene->mNumMeshes << std::endl;
    std::cout << "Materials: " << scene->mNumMaterials << std::endl;

    GameObject* rootObj = ProcessNode(scene->mRootNode, scene, directory);

    glm::vec3 minBounds(std::numeric_limits<float>::max());
    glm::vec3 maxBounds(std::numeric_limits<float>::lowest());
    glm::mat4 identity(1.0f);
    CalculateBoundingBox(rootObj, minBounds, maxBounds, identity);

    glm::vec3 size = maxBounds - minBounds;
    std::cout << "Dimensions: X=" << size.x << " Y=" << size.y << " Z=" << size.z << std::endl;

    std::string fileName = file_path.substr(file_path.find_last_of("/\\") + 1);
    std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);

    glm::quat correction = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    // Hardcoded rotation fix for BakerHouse model
    if (fileName.find("baker") != std::string::npos || fileName.find("house") != std::string::npos)
    {
        correction = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));
    }
    else
    {
        correction = DetectCorrectionRotation(scene, size);
    }

    glm::quat identity_quat(1.0f, 0.0f, 0.0f, 0.0f);
    float dot_product = glm::dot(correction, identity_quat);

    if (std::abs(dot_product - 1.0f) > 0.0001f)
    {
        Transform* rootTransform = static_cast<Transform*>(rootObj->GetComponent(ComponentType::TRANSFORM));
        if (rootTransform)
        {
            std::cout << "Applying rotation correction" << std::endl;
            glm::quat existing = rootTransform->GetRotationQuat();
            rootTransform->SetRotationQuat(correction * existing);
        }
    }
    else
    {
        std::cout << "No rotation correction needed" << std::endl;
    }

    // Normalize scale
    NormalizeModelScale(rootObj, 5.0f);

    aiReleaseImport(scene);
    return rootObj;
}

GameObject* FileSystem::ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory)
{
    std::string nodeName = node->mName.C_Str();
    if (nodeName.empty()) nodeName = "Unnamed";

    GameObject* gameObject = new GameObject(nodeName);

    LOG("Processing node: %s", nodeName.c_str());

    Transform* transform = static_cast<Transform*>(gameObject->GetComponent(ComponentType::TRANSFORM));

    if (transform != nullptr)
    {
        aiVector3D position, scaling;
        aiQuaternion rotation;
        node->mTransformation.Decompose(scaling, rotation, position);

        transform->SetPosition(glm::vec3(position.x, position.y, position.z));
        transform->SetScale(glm::vec3(scaling.x, scaling.y, scaling.z));
        transform->SetRotationQuat(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
    }

    // Process all meshes for this node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];
        aiMesh* aiMesh = scene->mMeshes[meshIndex];

        LOG("Processing mesh %d: %s", i, aiMesh->mName.C_Str());

        Mesh mesh = ProcessMesh(aiMesh, scene);

        ComponentMesh* meshComponent = static_cast<ComponentMesh*>(gameObject->CreateComponent(ComponentType::MESH));
        meshComponent->SetMesh(mesh);

        // Load diffuse textures if available
        if (aiMesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];

            if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
            {
                aiString texturePath;
                material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

                LOG("Material texture: %s", texturePath.C_Str());

                ComponentMaterial* matComponent = static_cast<ComponentMaterial*>(gameObject->GetComponent(ComponentType::MATERIAL));

                if (matComponent == nullptr)
                {
                    matComponent = static_cast<ComponentMaterial*>(gameObject->CreateComponent(ComponentType::MATERIAL));
                }

                std::string textureFile = texturePath.C_Str();
                std::string fileName;

                size_t lastSlash = textureFile.find_last_of("/\\");
                if (lastSlash != std::string::npos)
                    fileName = textureFile.substr(lastSlash + 1);
                else
                    fileName = textureFile;

                std::vector<std::string> searchPaths = {
                    directory + "\\" + fileName,
                    directory + "\\Textures\\" + fileName,
                    textureFile
                };

                bool loaded = false;
                for (const auto& path : searchPaths)
                {
                    LOG("Trying texture at: %s", path.c_str());
                    if (matComponent->LoadTexture(path))
                    {
                        LOG("Texture loaded successfully from: %s", path.c_str());
                        loaded = true;
                        break;
                    }
                }

                if (!loaded)
                {
                    LOG("Texture '%s' not found, using checkerboard", fileName.c_str());
                }
            }
        }
    }

    // Recursively process child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        GameObject* child = ProcessNode(node->mChildren[i], scene, directory);
        if (child != nullptr)
        {
            gameObject->AddChild(child);
        }
    }

    return gameObject;
}

Mesh FileSystem::ProcessMesh(aiMesh* aiMesh, const aiScene* scene)
{
    Mesh mesh;

    mesh.vertices.reserve(aiMesh->mNumVertices);
    mesh.indices.reserve(aiMesh->mNumFaces * 3);

    // Vertices
    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.position = glm::vec3(
            aiMesh->mVertices[i].x,
            aiMesh->mVertices[i].y,
            aiMesh->mVertices[i].z
        );

        if (aiMesh->HasNormals())
        {
            vertex.normal = glm::vec3(
                aiMesh->mNormals[i].x,
                aiMesh->mNormals[i].y,
                aiMesh->mNormals[i].z
            );
        }
        else
        {
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (aiMesh->HasTextureCoords(0))
        {
            vertex.texCoords = glm::vec2(
                aiMesh->mTextureCoords[0][i].x,
                aiMesh->mTextureCoords[0][i].y
            );
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        mesh.vertices.push_back(vertex);
    }

    // Indices
    for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace face = aiMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            mesh.indices.push_back(face.mIndices[j]);
        }
    }

    LOG("Vertices: %d, Indices: %d", mesh.vertices.size(), mesh.indices.size());

    return mesh;
}

void FileSystem::NormalizeModelScale(GameObject* rootObject, float targetSize)
{
    glm::vec3 minBounds(std::numeric_limits<float>::max());
    glm::vec3 maxBounds(std::numeric_limits<float>::lowest());

    glm::mat4 identity(1.0f);
    CalculateBoundingBox(rootObject, minBounds, maxBounds, identity);

    glm::vec3 size = maxBounds - minBounds;

    float maxDimension = std::max({ size.x, size.y, size.z });

    if (maxDimension > 0.0f)
    {
        float scale = targetSize / maxDimension;
        Transform* t = static_cast<Transform*>(rootObject->GetComponent(ComponentType::TRANSFORM));
        if (t != nullptr)
        {
            glm::vec3 currentScale = t->GetScale();
            t->SetScale(currentScale * scale);
        }

        std::cout << "Model normalized: " << maxDimension << " -> scale=" << scale << std::endl;
    }
}

void FileSystem::CalculateBoundingBox(GameObject* obj, glm::vec3& minBounds, glm::vec3& maxBounds, const glm::mat4& parentTransform)
{
    Transform* t = static_cast<Transform*>(obj->GetComponent(ComponentType::TRANSFORM));
    glm::mat4 localTransform(1.0f);

    if (t != nullptr)
    {
        glm::vec3 pos = t->GetPosition();
        glm::vec3 scale = t->GetScale();
        glm::quat rot = t->GetRotationQuat();

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos);
        glm::mat4 rotation = glm::mat4_cast(rot);
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

        localTransform = translation * rotation * scaleMatrix;
    }

    glm::mat4 worldTransform = parentTransform * localTransform;

    ComponentMesh* meshComp = static_cast<ComponentMesh*>(obj->GetComponent(ComponentType::MESH));
    if (meshComp != nullptr)
    {
        const Mesh& mesh = meshComp->GetMesh();

        for (const auto& vertex : mesh.vertices)
        {
            glm::vec4 worldPos = worldTransform * glm::vec4(vertex.position, 1.0f);
            glm::vec3 pos3(worldPos.x, worldPos.y, worldPos.z);

            minBounds.x = std::min(minBounds.x, pos3.x);
            minBounds.y = std::min(minBounds.y, pos3.y);
            minBounds.z = std::min(minBounds.z, pos3.z);

            maxBounds.x = std::max(maxBounds.x, pos3.x);
            maxBounds.y = std::max(maxBounds.y, pos3.y);
            maxBounds.z = std::max(maxBounds.z, pos3.z);
        }
    }

    for (GameObject* child : obj->GetChildren())
    {
        CalculateBoundingBox(child, minBounds, maxBounds, worldTransform);
    }
}

glm::quat FileSystem::DetectCorrectionRotation(const aiScene* scene, const glm::vec3& modelSize)
{
    std::cout << "\n=== Rotation Detection ===" << std::endl;
    std::cout << "Model size - X: " << modelSize.x << " Y: " << modelSize.y << " Z: " << modelSize.z << std::endl;

    glm::quat correction = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    // Try reading metadata first
    if (scene && scene->mMetaData)
    {
        std::cout << "Metadata found" << std::endl;

        int upAxis = 1;
        int upAxisSign = 1;

        bool hasUpAxis = scene->mMetaData->Get("UpAxis", upAxis);
        bool hasUpAxisSign = scene->mMetaData->Get("UpAxisSign", upAxisSign);

        std::cout << "UpAxis: " << upAxis << " (found: " << hasUpAxis << ")" << std::endl;
        std::cout << "UpAxisSign: " << upAxisSign << " (found: " << hasUpAxisSign << ")" << std::endl;

        if (upAxis == 2) // Z-up detected
        {
            std::cout << ">>> Z-up detected! Applying -90° X rotation" << std::endl;
            correction = glm::angleAxis(glm::radians(-90.0f * (float)upAxisSign), glm::vec3(1, 0, 0));
            return correction;
        }
        else if (upAxis == 1) // Y-up (OpenGL)
        {
            std::cout << ">>> Y-up detected, no correction needed" << std::endl;
        }
    }
    else
    {
        std::cout << "No metadata, using heuristic detection" << std::endl;
    }

    float yzRatio = (modelSize.y > 0.0001f) ? (modelSize.z / modelSize.y) : 0.0f;

    std::cout << "YZ Ratio: " << yzRatio << " (threshold: 1.4)" << std::endl;

    if (yzRatio > 1.4f)
    {
        std::cout << ">>> Heuristic: Z dimension dominant, applying -90° X rotation" << std::endl;
        correction = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0));
    }
    else
    {
        std::cout << ">>> No correction applied" << std::endl;
    }

    std::cout << "=======================\n" << std::endl;
    return correction;
}
