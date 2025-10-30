#pragma once

#include "Component.h"
#include "FileSystem.h" 

class ComponentMesh : public Component {
public:
    ComponentMesh(GameObject* owner);
    ~ComponentMesh();

    void Update() override;
    void OnEditor() override;

    // Set mesh data and upload to GPU
    void SetMesh(const Mesh& meshData);

    // Mesh access
    const Mesh& GetMesh() const { return mesh; }
    Mesh& GetMesh() { return mesh; }

    // Validation
    bool HasMesh() const { return mesh.IsValid(); }

    // Mesh statistics
    unsigned int GetNumVertices() const { return static_cast<unsigned int>(mesh.vertices.size()); }
    unsigned int GetNumIndices() const { return static_cast<unsigned int>(mesh.indices.size()); }
    unsigned int GetNumTriangles() const { return GetNumIndices() / 3; }
    unsigned int GetNumTextures() const { return static_cast<unsigned int>(mesh.textures.size()); }

private:
    Mesh mesh;
};