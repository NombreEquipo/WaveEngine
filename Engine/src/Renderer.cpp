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


Renderer::Renderer()
{
    LOG("Renderer Constructor");
    camera = make_unique<Camera>();
}

Renderer::~Renderer()
{
}

bool Renderer::Start()
{
    LOG("Initializing Renderer");

    // Create default shader
    defaultShader = make_unique<Shader>();

    if (!defaultShader->Create())
    {
        LOG("Failed to create default shader");
        return false;
    }

    // Crear textura desde archivo
    defaultTexture = make_unique<Texture>();

    if (!defaultTexture->LoadFromFile("Assets\\pruebas.png"))
    {
        LOG("Failed to load texture from file, using checkerboard");
        defaultTexture->CreateCheckerboard();
    }
    else
    {
        LOG("Texture loaded successfully from file");
    }

    LOG("Renderer initialized successfully");

    // for testing
    sphere = Primitives::CreateSphere();
    LoadMesh(sphere);
    cylinder = Primitives::CreateCylinder();
    LoadMesh(cylinder);
    pyramid = Primitives::CreatePyramid();
    LoadMesh(pyramid);

    return true;
}

void Renderer::LoadMesh(Mesh& mesh)
{
    unsigned int VAO, VBO, EBO, texVBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO for vertices
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.num_vertices * 3 * sizeof(float), mesh.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // VBO for texture coordinates
    if (mesh.texCoords != nullptr)
    {
        glGenBuffers(1, &texVBO);
        glBindBuffer(GL_ARRAY_BUFFER, texVBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.num_vertices * 2 * sizeof(float), mesh.texCoords, GL_STATIC_DRAW);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        mesh.id_texcoord = texVBO;
    }

    // EBO for indices
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.num_indices * sizeof(unsigned int), mesh.indices, GL_STATIC_DRAW);

    mesh.id_vertex = VBO;
    mesh.id_index = EBO;
    mesh.id_VAO = VAO;

    glBindVertexArray(0);
}

void Renderer::DrawMesh(const Mesh& mesh)
{
    if (mesh.id_VAO == 0)
    {
        LOG("ERROR: Trying to draw mesh without VAO");
        return;
    }

    glBindVertexArray(mesh.id_VAO);
    glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Renderer::UnloadMesh(Mesh& mesh)
{
    if (mesh.id_VAO != 0)
    {
        glDeleteVertexArrays(1, &mesh.id_VAO);
        mesh.id_VAO = 0;
    }

    if (mesh.id_vertex != 0)
    {
        glDeleteBuffers(1, &mesh.id_vertex);
        mesh.id_vertex = 0;
    }

    if (mesh.id_index != 0)
    {
        glDeleteBuffers(1, &mesh.id_index);
        mesh.id_index = 0;
    }

    if (mesh.id_texcoord != 0)
    {
        glDeleteBuffers(1, &mesh.id_texcoord);
        mesh.id_texcoord = 0;
    }
}

void Renderer::LoadTexture(const std::string& path)
{
    auto newTexture = make_unique<Texture>();

    if (newTexture->LoadFromFile(path))
    {
        defaultTexture = std::move(newTexture);
        std::cout << "Texture applied successfully: " << path << std::endl;
    }
    else
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
}

bool Renderer::PreUpdate()
{
    return true;
}

bool Renderer::Update()
{
    glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    defaultShader->Use();

    camera->Update();

    int width, height;
    Application::GetInstance().window->GetWindowSize(width, height);

    // Update aspect ratio 
    float aspectRatio = (float)width / (float)height;
    camera->SetAspectRatio(aspectRatio);

    GLuint shaderProgram = defaultShader->GetProgramID();

    // send matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

    // activate and bind texture 
    glActiveTexture(GL_TEXTURE0);

    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    GameObject* root = Application::GetInstance().scene->GetRoot();

    if (root != nullptr && root->GetChildren().size() > 0)
    {
        DrawScene();
    }
    else
    {
     
        defaultTexture->Bind();

        const vector<Mesh>& meshes = Application::GetInstance().filesystem->GetMeshes();

        if (!meshes.empty())
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

            for (const auto& mesh : meshes)
            {
                DrawMesh(mesh);
            }
        }
        else
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),1, GL_FALSE,glm::value_ptr(modelMatrix));

            DrawMesh(pyramid);
        }

        defaultTexture->Unbind();
    }
     
    defaultTexture->Unbind();

    return true;
}

bool Renderer::CleanUp()
{
    LOG("Cleaning up Renderer");

    if (defaultShader)
    {
        defaultShader->Delete();
    }

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

	// Global matrix 
    glm::mat4 modelMatrix = transform->GetGlobalMatrix();

	// Send matrix to shader
    GLuint shaderProgram = defaultShader->GetProgramID();
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    ComponentMaterial* material = static_cast<ComponentMaterial*>(gameObject->GetComponent(ComponentType::MATERIAL));

	// Check if material exists and is active
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
        }
    }

	// Desbind material or default texture
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

