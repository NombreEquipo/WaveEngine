#pragma once
#include "Module.h"
#include "FileSystem.h"
#include "Shaders.h"
#include "Texture.h"
#include <memory>
#include "Primitives.h"
#include "Camera.h"

class GameObject;

class Renderer : public Module
{
public:
    Renderer();
    ~Renderer();

    bool Start() override;
    bool Update() override;
    bool CleanUp() override;
    bool PreUpdate() override;

    void LoadMesh(Mesh& mesh);
    void DrawMesh(const Mesh& mesh);
    void UnloadMesh(Mesh& mesh);
    void LoadTexture(const std::string& path);

    void DrawScene();
    void DrawGameObject(GameObject* gameObject);


    Shader* GetDefaultShader() const { return defaultShader.get(); }
    Camera* GetCamera() { return camera.get(); }

private:

    void DrawGameObjectRecursive(GameObject* gameObject);

    std::unique_ptr<Shader> defaultShader;
    std::unique_ptr<Texture> defaultTexture;
    Mesh sphere, cube, pyramid, cylinder, plane;
    unique_ptr<Camera> camera;
};