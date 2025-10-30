#include "ComponentMesh.h"
#include "GameObject.h"
#include "Application.h"
#include <limits>
#include <algorithm>  

ComponentMesh::ComponentMesh(GameObject* owner)
    : Component(owner, ComponentType::MESH),
    aabbMin(0.0f), aabbMax(0.0f)
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

    // Calculate AABB
    CalculateAABB();

    // Upload to GPU
    Application::GetInstance().renderer->LoadMesh(mesh);
}

void ComponentMesh::CalculateAABB()
{
    // If the mesh has no vertices, set the AABB to zero
    if (mesh.vertices.empty())
    {
        aabbMin = glm::vec3(0.0f);
        aabbMax = glm::vec3(0.0f);
        return;
    }

    // Initialize min and max with the first vertex position
    aabbMin = mesh.vertices[0].position;
    aabbMax = mesh.vertices[0].position;

    // Iterate through all vertices to find the minimum and maximum coordinates
    for (const auto& vertex : mesh.vertices)
    {
        aabbMin.x = std::min(aabbMin.x, vertex.position.x);
        aabbMin.y = std::min(aabbMin.y, vertex.position.y);
        aabbMin.z = std::min(aabbMin.z, vertex.position.z);

        aabbMax.x = std::max(aabbMax.x, vertex.position.x);
        aabbMax.y = std::max(aabbMax.y, vertex.position.y);
        aabbMax.z = std::max(aabbMax.z, vertex.position.z);
    }
}
