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

    void DrawVertexNormals(const Mesh& mesh, const glm::mat4& modelMatrix);
    void DrawFaceNormals(const Mesh& mesh, const glm::mat4& modelMatrix);

    Shader* GetDefaultShader() const { return defaultShader.get(); }
    Camera* GetCamera() { return camera.get(); }


	// Render settings
    bool IsDepthTestEnabled() const { return depthTestEnabled; }
    void SetDepthTest(bool enabled);

    bool IsFaceCullingEnabled() const { return faceCullingEnabled; }
    void SetFaceCulling(bool enabled);

    bool IsWireframeMode() const { return wireframeMode; }
    void SetWireframeMode(bool enabled);

    void GetClearColor(float& r, float& g, float& b) const { r = clearColorR; g = clearColorG; b = clearColorB; }
    void SetClearColor(float r, float g, float b);

    int GetCullFaceMode() const { return cullFaceMode; } // 0=Back, 1=Front, 2=Both
    void SetCullFaceMode(int mode);


private:

    void DrawGameObjectRecursive(GameObject* gameObject);
    void ApplyRenderSettings();

    std::unique_ptr<Shader> defaultShader;
    std::unique_ptr<Shader> lineShader;

    std::unique_ptr<Texture> defaultTexture;
    Mesh sphere, cube, pyramid, cylinder, plane;
    unique_ptr<Camera> camera;

    // Renderer Configuration
    bool depthTestEnabled = true;
    bool faceCullingEnabled = true;
    bool wireframeMode = false;
    float clearColorR = 0.2f;
    float clearColorG = 0.25f;
    float clearColorB = 0.3f;
    int cullFaceMode = 0; // 0=GL_BACK, 1=GL_FRONT, 2=GL_FRONT_AND_BACK

    GLuint normalLinesVAO = 0;
    GLuint normalLinesVBO = 0;
    size_t normalLinesCapacity = 0;

    struct ShaderUniforms {
        GLint projection = -1;
        GLint view = -1;
        GLint model = -1;
        GLint texture1 = -1;
    } defaultUniforms, lineUniforms;
};