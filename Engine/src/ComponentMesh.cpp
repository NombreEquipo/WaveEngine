#include "ComponentMesh.h"
#include "GameObject.h"
#include "Application.h"

ComponentMesh::ComponentMesh(GameObject* owner)
    : Component(owner, ComponentType::MESH)
{
}

ComponentMesh::~ComponentMesh()
{
    if (HasMesh())
    {
        Application::GetInstance().renderer->UnloadMesh(mesh);
    }
}

void ComponentMesh::Update()
{
    // Reserved for future mesh updates
}

void ComponentMesh::OnEditor()
{
    // Reserved for ImGui editor interface
}

void ComponentMesh::SetMesh(const Mesh& meshData)
{
    // Unload previous mesh if exists
    if (HasMesh())
    {
        Application::GetInstance().renderer->UnloadMesh(mesh);
    }

    // Copy mesh data
    mesh.vertices = meshData.vertices;
    mesh.indices = meshData.indices;
    mesh.textures = meshData.textures;

    // Reset OpenGL handles (will be set by renderer)
    mesh.VAO = 0;
    mesh.VBO = 0;
    mesh.EBO = 0;

    // Upload to GPU
    Application::GetInstance().renderer->LoadMesh(mesh);
}