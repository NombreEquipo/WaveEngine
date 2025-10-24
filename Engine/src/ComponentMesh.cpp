#include "ComponentMesh.h"
#include "GameObject.h"
#include "Application.h"

ComponentMesh::ComponentMesh(GameObject* owner)
    : Component(owner, ComponentType::MESH)
{
    mesh.num_vertices = 0;
    mesh.vertices = nullptr;
    mesh.num_indices = 0;
    mesh.indices = nullptr;
    mesh.texCoords = nullptr;
    mesh.id_vertex = 0;
    mesh.id_index = 0;
    mesh.id_texcoord = 0;
    mesh.id_VAO = 0;
}

ComponentMesh::~ComponentMesh()
{
    if (HasMesh())
    {
        Application::GetInstance().renderer->UnloadMesh(mesh);

        delete[] mesh.vertices;
        delete[] mesh.indices;
        delete[] mesh.texCoords;

        mesh.vertices = nullptr;
        mesh.indices = nullptr;
        mesh.texCoords = nullptr;
    }
}

void ComponentMesh::Update()
{

}

void ComponentMesh::OnEditor()
{

}

void ComponentMesh::SetMesh(const Mesh& meshData)
{
    if (HasMesh())
    {
        Application::GetInstance().renderer->UnloadMesh(mesh);
        delete[] mesh.vertices;
        delete[] mesh.indices;
        delete[] mesh.texCoords;
    }

    mesh.num_vertices = meshData.num_vertices;
    mesh.num_indices = meshData.num_indices;

    if (meshData.vertices != nullptr && meshData.num_vertices > 0)
    {
        mesh.vertices = new float[meshData.num_vertices * 3];
        memcpy(mesh.vertices, meshData.vertices, meshData.num_vertices * 3 * sizeof(float));
    }
    else
    {
        mesh.vertices = nullptr;
    }

    if (meshData.indices != nullptr && meshData.num_indices > 0)
    {
        mesh.indices = new unsigned int[meshData.num_indices];
        memcpy(mesh.indices, meshData.indices, meshData.num_indices * sizeof(unsigned int));
    }
    else
    {
        mesh.indices = nullptr;
    }

    if (meshData.texCoords != nullptr && meshData.num_vertices > 0)
    {
        mesh.texCoords = new float[meshData.num_vertices * 2];
        memcpy(mesh.texCoords, meshData.texCoords, meshData.num_vertices * 2 * sizeof(float));
    }
    else
    {
        mesh.texCoords = nullptr;
    }

    Application::GetInstance().renderer->LoadMesh(mesh);
}