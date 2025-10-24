#pragma once

#include "Component.h"
#include "FileSystem.h" 

class ComponentMesh : public Component {
public:
    ComponentMesh(GameObject* owner);
    ~ComponentMesh();

    void Update() override;
    void OnEditor() override;

    void SetMesh(const Mesh& meshData);

    const Mesh& GetMesh() const { return mesh; }
    Mesh& GetMesh() { return mesh; }

    bool HasMesh() const { return mesh.id_VAO != 0; }

    unsigned int GetNumVertices() const { return mesh.num_vertices; }
    unsigned int GetNumIndices() const { return mesh.num_indices; }
    unsigned int GetNumTriangles() const { return mesh.num_indices / 3; }

private:
    Mesh mesh;
};