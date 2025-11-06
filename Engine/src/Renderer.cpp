#include "Renderer.h"
#include "Application.h"
#include <glad/glad.h>
#include <iostream>
#include "Texture.h"
#include "Shaders.h"
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ModuleEditor.h"

Renderer::Renderer()
{
    LOG_DEBUG("Renderer Constructor");
    camera = make_unique<Camera>();
}

Renderer::~Renderer()
{
}

bool Renderer::Start()
{
    LOG_DEBUG("=== Initializing Renderer Module ===");
    LOG_CONSOLE("Initializing renderer and shaders...");

    // Configuración OpenGL para 3D
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Create default shader
    defaultShader = make_unique<Shader>();

    if (!defaultShader->Create())
    {
        LOG_DEBUG("ERROR: Failed to create default shader");
        LOG_CONSOLE("ERROR: Failed to compile shaders");
        return false;
    }
    else
    {
        LOG_DEBUG("Shader created successfully - Program ID: %d", defaultShader->GetProgramID());
        LOG_CONSOLE("OpenGL shaders compiled successfully");
    }

    lineShader = make_unique<Shader>();

    if (!lineShader->CreateSimpleColor())
    {
        LOG_DEBUG("ERROR: Failed to create line shader");
        LOG_CONSOLE("ERROR: Failed to compile line shader");
        return false;
    }
    else
    {
        LOG_DEBUG("Line shader created successfully - Program ID: %d", lineShader->GetProgramID());
        LOG_CONSOLE("Line shader compiled successfully");
    }

    // Create default CHECKERBOARD texture (para objetos sin material)
    defaultTexture = make_unique<Texture>();
    defaultTexture->CreateCheckerboard();
    LOG_DEBUG("Default checkerboard texture created");

    LOG_DEBUG("Renderer initialized successfully");
    LOG_CONSOLE("Renderer ready");

    return true;
}
void Renderer::LoadMesh(Mesh& mesh)
{
    // Generate VAO
    glGenVertexArrays(1, &mesh.VAO);
    glBindVertexArray(mesh.VAO);

    // Generate and bind VBO for vertices
    glGenBuffers(1, &mesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normal attribute (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // Texture coordinate attribute (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // Generate and bind EBO for indices
    glGenBuffers(1, &mesh.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    LOG_DEBUG("Mesh loaded - VAO: %d, Vertices: %d, Indices: %d", mesh.VAO, mesh.vertices.size(), mesh.indices.size());
}

void Renderer::DrawMesh(const Mesh& mesh)
{
    if (mesh.VAO == 0)
    {
        LOG_DEBUG("ERROR: Trying to draw mesh without VAO");
        LOG_CONSOLE("Render error: Invalid mesh");
        return;
    }

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Renderer::UnloadMesh(Mesh& mesh)
{
    if (mesh.VAO != 0)
    {
        glDeleteVertexArrays(1, &mesh.VAO);
        mesh.VAO = 0;
    }

    if (mesh.VBO != 0)
    {
        glDeleteBuffers(1, &mesh.VBO);
        mesh.VBO = 0;
    }

    if (mesh.EBO != 0)
    {
        glDeleteBuffers(1, &mesh.EBO);
        mesh.EBO = 0;
    }
}

void Renderer::LoadTexture(const std::string& path)
{

    LOG_DEBUG("Renderer: Loading new texture");
    LOG_CONSOLE("Loading texture...");

    auto newTexture = make_unique<Texture>();

    if (newTexture->LoadFromFile(path))
    {
        defaultTexture = std::move(newTexture);
        LOG_DEBUG("Renderer: Texture applied successfully");
        LOG_CONSOLE("Texture applied to scene");
    }
    else
    {
        LOG_DEBUG("ERROR: Renderer failed to load texture: %s", path.c_str());
        LOG_CONSOLE("Failed to apply texture");
    }
}

bool Renderer::PreUpdate()
{
    return true;
}

bool Renderer::Update()
{
    glClearColor(clearColorR, clearColorG, clearColorB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    defaultShader->Use();

    camera->Update();

    int width, height;
    Application::GetInstance().window->GetWindowSize(width, height);

    // Update aspect ratio 
    float aspectRatio = (float)width / (float)height;
    camera->SetAspectRatio(aspectRatio);

    GLuint shaderProgram = defaultShader->GetProgramID();

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

    // Activate and bind default texture 
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    GameObject* root = Application::GetInstance().scene->GetRoot();

    if (root != nullptr && root->GetChildren().size() > 0)
    {
        DrawScene();
    }

    defaultTexture->Unbind();

    return true;
}

bool Renderer::CleanUp()
{
    LOG_DEBUG("Cleaning up Renderer");

    // Unload test primitives
    UnloadMesh(sphere);
    UnloadMesh(cylinder);
    UnloadMesh(pyramid);

    if (defaultShader)
    {
        defaultShader->Delete();
    }

    LOG_DEBUG("Renderer cleaned up successfully");
    LOG_CONSOLE("Renderer shutdown complete");

    return true;
}

void Renderer::DrawScene()
{
    GameObject* root = Application::GetInstance().scene->GetRoot();

    if (root == nullptr)
        return;

    DrawGameObjectRecursive(root);
}

void Renderer::DrawGameObject(GameObject* gameObject)
{
    if (gameObject == nullptr || !gameObject->IsActive())
        return;

    DrawGameObjectRecursive(gameObject);
}

void Renderer::DrawGameObjectRecursive(GameObject* gameObject)
{
    if (!gameObject->IsActive())
        return;

    Transform* transform = static_cast<Transform*>(gameObject->GetComponent(ComponentType::TRANSFORM));
    if (transform == nullptr) return;

    glm::mat4 modelMatrix = transform->GetGlobalMatrix();

    GLuint shaderProgram = defaultShader->GetProgramID();
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    //Selection tint============================
    SelectionManager* selectionMgr = Application::GetInstance().selectionManager;

    if (selectionMgr->IsSelected(gameObject))
    {
        defaultShader->SetVec3("tintColor", glm::vec3(1.0f, 0.75f, 0.8f));
        // yellow
    }
    else
    {
        defaultShader->SetVec3("tintColor", glm::vec3(1.0f, 1.0f, 1.0f)); // Normal
    }
    // ==========================================

    ComponentMaterial* material = static_cast<ComponentMaterial*>(gameObject->GetComponent(ComponentType::MATERIAL));

    if (material != nullptr && material->IsActive())
    {
        material->Use();
    }
    else
    {
        defaultTexture->Bind();
    }

    // Draw all mesh components
    std::vector<Component*> meshComponents = gameObject->GetComponentsOfType(ComponentType::MESH);

    for (Component* comp : meshComponents)
    {
        ComponentMesh* meshComp = static_cast<ComponentMesh*>(comp);

        if (meshComp->IsActive() && meshComp->HasMesh())
        {
            const Mesh& mesh = meshComp->GetMesh();
            DrawMesh(mesh);

            ModuleEditor* editor = Application::GetInstance().editor.get();
            SelectionManager* selectionManager = Application::GetInstance().selectionManager;

            if (editor != nullptr && selectionManager != nullptr)
            {
                bool shouldDrawNormals = false;
                if (selectionMgr->IsSelected(gameObject))
                {
                    shouldDrawNormals = true;
                }
                else
                {
                    GameObject* parent = gameObject->GetParent();
                    while (parent != nullptr)
                    {
                        if (selectionMgr->IsSelected(parent))
                        {
                            shouldDrawNormals = true;
                            break;
                        }
                        parent = parent->GetParent();
                    }
                }
                if (shouldDrawNormals)
                {
                    if (editor->ShouldShowVertexNormals())
                    {
                        DrawVertexNormals(mesh, modelMatrix);
                    }
                    if (editor->ShouldShowFaceNormals())
                    {
                        DrawFaceNormals(mesh, modelMatrix);
                    }
                }
            }
        }
    }

    // Unbind material or default texture
    if (material != nullptr && material->IsActive())
    {
        material->Unbind();
    }
    else
    {
        defaultTexture->Unbind();
    }

    // Recursively draw children
    const std::vector<GameObject*>& children = gameObject->GetChildren();
    for (GameObject* child : children)
    {
        DrawGameObjectRecursive(child);
    }
}


void Renderer::DrawVertexNormals(const Mesh& mesh, const glm::mat4& modelMatrix)
{
    if (!mesh.IsValid() || mesh.vertices.empty())
        return;

	std::vector<float> lineVertices; // Store line vertex positions
	float normalLength = 0.2f; // <== We can change the length with this ==>

    for (const auto& vertex : mesh.vertices)
    {
        glm::vec4 worldPos = modelMatrix * glm::vec4(vertex.position, 1.0f);

		// Transform normal to world space
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        glm::vec3 worldNormal = glm::normalize(normalMatrix * vertex.normal);

		// End point of the normal line
        glm::vec3 endPoint = glm::vec3(worldPos) + worldNormal * normalLength;

        // Start point
        lineVertices.push_back(worldPos.x);
        lineVertices.push_back(worldPos.y);
        lineVertices.push_back(worldPos.z);

        // End point
        lineVertices.push_back(endPoint.x);
        lineVertices.push_back(endPoint.y);
        lineVertices.push_back(endPoint.z);
    }

    GLuint lineVAO, lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(float), lineVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    lineShader->Use();
    GLuint shaderProgram = lineShader->GetProgramID();

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); 

	lineShader->SetVec3("color", glm::vec3(0.0f, 0.5f, 1.0f)); // Can be changed to whatever color

    glDrawArrays(GL_LINES, 0, lineVertices.size() / 3);

    glBindVertexArray(0);
    glDeleteBuffers(1, &lineVBO);
    glDeleteVertexArrays(1, &lineVAO);

    defaultShader->Use();
}

void Renderer::DrawFaceNormals(const Mesh& mesh, const glm::mat4& modelMatrix)
{
    if (!mesh.IsValid() || mesh.vertices.empty() || mesh.indices.empty())
        return;

    std::vector<float> lineVertices; // Store line vertex positions
    float normalLength = 0.3f;  // <== We can change the length with this ==>

    for (size_t i = 0; i < mesh.indices.size(); i += 3)
    {
		// Get the vertices 
        const Vertex& v0 = mesh.vertices[mesh.indices[i]];
        const Vertex& v1 = mesh.vertices[mesh.indices[i + 1]];
        const Vertex& v2 = mesh.vertices[mesh.indices[i + 2]];

		// Transform normal to world space
        glm::vec4 worldPos0 = modelMatrix * glm::vec4(v0.position, 1.0f);
        glm::vec4 worldPos1 = modelMatrix * glm::vec4(v1.position, 1.0f);
        glm::vec4 worldPos2 = modelMatrix * glm::vec4(v2.position, 1.0f);

		// Calculate the center of the face
        glm::vec3 faceCenter = (glm::vec3(worldPos0) + glm::vec3(worldPos1) + glm::vec3(worldPos2)) / 3.0f;

        glm::vec3 edge1 = glm::vec3(worldPos1) - glm::vec3(worldPos0);
        glm::vec3 edge2 = glm::vec3(worldPos2) - glm::vec3(worldPos0);
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        // End point of the normal line
        glm::vec3 endPoint = faceCenter + faceNormal * normalLength;

        // Start point
        lineVertices.push_back(faceCenter.x);
        lineVertices.push_back(faceCenter.y);
        lineVertices.push_back(faceCenter.z);

        // End point
        lineVertices.push_back(endPoint.x);
        lineVertices.push_back(endPoint.y);
        lineVertices.push_back(endPoint.z);
    }

    GLuint lineVAO, lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(float), lineVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    lineShader->Use();
    GLuint shaderProgram = lineShader->GetProgramID();

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); 

	lineShader->SetVec3("color", glm::vec3(0.0f, 1.0f, 0.5f)); // Can be changed to whatever color

    glDrawArrays(GL_LINES, 0, lineVertices.size() / 3);

    glBindVertexArray(0);
    glDeleteBuffers(1, &lineVBO);
    glDeleteVertexArrays(1, &lineVAO);

    defaultShader->Use();
}

void Renderer::SetDepthTest(bool enabled)
{
    depthTestEnabled = enabled;
    if (enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    LOG_DEBUG("Depth test %s", enabled ? "enabled" : "disabled");
}

void Renderer::SetFaceCulling(bool enabled)
{
    faceCullingEnabled = enabled;
    if (enabled)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    LOG_DEBUG("Face culling %s", enabled ? "enabled" : "disabled");
}

void Renderer::SetWireframeMode(bool enabled)
{
    wireframeMode = enabled;
    if (enabled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    LOG_DEBUG("Wireframe mode %s", enabled ? "enabled" : "disabled");
}

void Renderer::SetClearColor(float r, float g, float b)
{
    clearColorR = r;
    clearColorG = g;
    clearColorB = b;
    LOG_DEBUG("Clear color set to (%.2f, %.2f, %.2f)", r, g, b);
}

void Renderer::SetCullFaceMode(int mode)
{
    cullFaceMode = mode;

    switch (mode)
    {
    case 0: glCullFace(GL_BACK); break;
    case 1: glCullFace(GL_FRONT); break;
    case 2: glCullFace(GL_FRONT_AND_BACK); break;
    default: glCullFace(GL_BACK); break;
    }

    const char* modeStr[] = { "Back", "Front", "Front and Back" };
    LOG_DEBUG("Cull face mode set to: %s", modeStr[mode]);
}

void Renderer::ApplyRenderSettings()
{
    // Apply all render settings
    SetDepthTest(depthTestEnabled);
    SetFaceCulling(faceCullingEnabled);
    SetWireframeMode(wireframeMode);
    SetCullFaceMode(cullFaceMode);
}
