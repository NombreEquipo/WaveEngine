#include "ComponentParticleSystem.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ComponentCamera.h"
#include "Transform.h"
#include "ModuleResources.h" 
#include "ResourceTexture.h"
#include "LibraryManager.h"
#include "Time.h" 
#include <fstream>
#include <iomanip> 
#include <imgui.h>
#include <glad/glad.h> 
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

ComponentParticleSystem::ComponentParticleSystem(GameObject* owner)
    : Component(owner, ComponentType::PARTICLE)
{
    name = "Particle System";
    emitter = new EmitterInstance();
    emitter->Init(); // Initialize default modules
}

ComponentParticleSystem::~ComponentParticleSystem() {
    // Release texture resource reference
    if (textureResourceUID != 0) {
        Application::GetInstance().resources->ReleaseResource(textureResourceUID);
    }
    // CleanUp memory
    if (emitter) {
        delete emitter;
        emitter = nullptr;
    }
}

void ComponentParticleSystem::Update() {
    // Update feedback message timer
    if (feedbackTimer > 0.0f) {
        feedbackTimer -= Application::GetInstance().time->GetRealDeltaTime();
    }
    // Only update simulation on PLAY mode
    if (Application::GetInstance().GetPlayState() != Application::PlayState::PLAYING) return;

    float dt = Application::GetInstance().time->GetDeltaTime();
    // Sync Emitter Position with GameObject Transform
    Transform* trans = dynamic_cast<Transform*>(owner->GetComponent(ComponentType::TRANSFORM));
    if (trans) {
        const glm::mat4& globalMatrix = trans->GetGlobalMatrix();
        glm::vec3 globalPos = glm::vec3(globalMatrix[3]); // Extract translation
        // Update Spawner module position
        for (auto mod : emitter->modules) {
            if (mod->type == ParticleModuleType::SPAWNER) {
                static_cast<ModuleEmitterSpawn*>(mod)->spawnPosition = globalPos;
            }
        }
    }
    emitter->Update(dt);
}

void ComponentParticleSystem::Draw(ComponentCamera* camera) {
    if (!active || !emitter) return;
    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Setup Legacy Matrices
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(camera->GetProjectionMatrix()));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(camera->GetViewMatrix()));
    // Draw particles
    emitter->Draw(camera->GetPosition());
}

void ComponentParticleSystem::OnEditor() {
    if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Control button
        ImGui::Checkbox("Active", &active);
        ImGui::SameLine();
        if (ImGui::Button("Reset Simulation")) emitter->Reset(); // Clears particles in the scene
        ImGui::SameLine();
        if (ImGui::Button("Reset Values")) emitter->ResetValues(); // Resets to default

        // Selector Texture
        ImGui::Separator();
        ImGui::Text("Texture File");
        std::string currentTexName = emitter->texturePath.empty() ? "None" : emitter->texturePath;
        // Strip path to show only filename
        size_t lastSlash = currentTexName.find_last_of("/\\");
        if (lastSlash != std::string::npos) currentTexName = currentTexName.substr(lastSlash + 1);

        if (ImGui::BeginCombo("Texture", currentTexName.c_str())) {
            const auto& allResources = Application::GetInstance().resources->GetAllResources();
            if (ImGui::Selectable("None", emitter->texturePath.empty())) SetTexture("");
            for (const auto& pair : allResources) {
                Resource* res = pair.second;
                if (res->GetType() == Resource::TEXTURE) {
                    std::string path = res->GetAssetFile();
                    std::string name = path;
                    size_t slash = name.find_last_of("/\\");
                    if (slash != std::string::npos) name = name.substr(slash + 1);
                    bool isSelected = (emitter->texturePath == path);
                    if (ImGui::Selectable(name.c_str(), isSelected)) SetTexture(path);
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Animation Settings
        ImGui::Text("Texture Animation");
        ImGui::DragInt("Rows", &emitter->textureRows, 0.1f, 1, 16);
        ImGui::DragInt("Columns", &emitter->textureCols, 0.1f, 1, 16);
        ImGui::DragFloat("Animation Cycles", &emitter->animationSpeed, 0.05f, 0.0f, 10.0f, "%.2fx");
        ImGui::Checkbox("Loop Animation", &emitter->animLoop);

        // Global Settings
        ImGui::Separator();
        ImGui::Text("Global Settings");
        ImGui::DragInt("Max Particles", &emitter->maxParticles, 1, 0, 10000);
        ImGui::DragFloat("Emission Rate", &emitter->emissionRate, 0.1f, 0.0f, 1000.0f);

        // Burst Button
        ImGui::Separator();
        static int burstAmount = 50;
        ImGui::DragInt("Burst Amount", &burstAmount, 1, 1, 500);
        if (ImGui::Button("BURST!")) {
            emitter->Burst(burstAmount);
        }
        ImGui::Checkbox("Additive Blending (Glow)", &emitter->additiveBlending);

        // Module settings
        for (auto mod : emitter->modules) {
            ImGui::Separator();
            if (mod->type == ParticleModuleType::SPAWNER) {
                ModuleEmitterSpawn* spawn = static_cast<ModuleEmitterSpawn*>(mod);
                ImGui::Text("Spawner Module");

                // Shape Selection
                const char* shapeItems[] = { "Box", "Sphere", "Cone" };
                int currentShape = static_cast<int>(spawn->shape);
                if (ImGui::Combo("Emitter Shape", &currentShape, shapeItems, IM_ARRAYSIZE(shapeItems))) {
                    spawn->shape = static_cast<EmitterShape>(currentShape);
                }

                if (spawn->shape == EmitterShape::BOX) {
                    ImGui::DragFloat3("Box Area", &spawn->emissionArea.x, 0.1f, 0.0f, 10.0f);
                }
                else {
                    ImGui::DragFloat("Radius", &spawn->emissionRadius, 0.1f, 0.0f, 10.0f);
                    if (spawn->shape == EmitterShape::CONE) {
                        ImGui::SliderFloat("Cone Angle", &spawn->coneAngle, 0.0f, 90.0f);
                    }
                }

                ImGui::Text("Lifecycle");
                ImGui::DragFloat2("Lifetime (Min/Max)", &spawn->lifetimeMin, 0.1f, 0.1f, 10.0f);

                // Color
                ImGui::Text("Color Over Lifetime");
                ImGui::PushID("ColorStart");
                ImGui::Text("Start:"); ImGui::SameLine();
                ImGui::ColorEdit3("##Color", &spawn->colorStart.r);
                ImGui::SameLine(); ImGui::SetNextItemWidth(60);
                ImGui::DragFloat("Alpha", &spawn->colorStart.a, 0.01f, 0.0f, 1.0f);
                ImGui::PopID();

                ImGui::PushID("ColorEnd");
                ImGui::Text("End:  "); ImGui::SameLine();
                ImGui::ColorEdit3("##Color", &spawn->colorEnd.r);
                ImGui::SameLine(); ImGui::SetNextItemWidth(60);
                ImGui::DragFloat("Alpha", &spawn->colorEnd.a, 0.01f, 0.0f, 1.0f);
                ImGui::PopID();

                ImGui::Text("Size Over Lifetime");
                ImGui::DragFloat("Size Start", &spawn->sizeStart, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("Size End", &spawn->sizeEnd, 0.01f, 0.0f, 10.0f);

                ImGui::Text("Movement");
                ImGui::DragFloat2("Speed (Min/Max)", &spawn->speedMin, 0.1f, 0.0f, 50.0f);

                ImGui::Text("Rotation");
                ImGui::DragFloat2("Spin Speed (Min/Max)", &spawn->rotationSpeedMin, 1.0f, -360.0f, 360.0f);
            }
            else if (mod->type == ParticleModuleType::MOVEMENT) {
                ModuleEmitterMovement* move = static_cast<ModuleEmitterMovement*>(mod);
                ImGui::Text("Physics");
                ImGui::DragFloat3("Gravity", &move->gravity.x, 0.1f);
            }
        }

        // Presets
        ImGui::Separator();
        ImGui::Text("Presets (.particle)");
        static char buf[64] = "new_effect";
        ImGui::InputText("Filename", buf, 64);

        // Path using LibraryManager
        std::string assetsRoot = LibraryManager::GetAssetsRoot();
        if (assetsRoot.back() != '/' && assetsRoot.back() != '\\') assetsRoot += "/";
        std::string particleFolder = assetsRoot + "Particles/";

        if (ImGui::Button("Save Preset")) {
            std::string path = particleFolder + std::string(buf) + ".particle";
            SaveParticleFile(path);
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Preset")) {
            std::string path = particleFolder + std::string(buf) + ".particle";
            LoadParticleFile(path);
        }
        if (feedbackTimer > 0.0f) {
            ImGui::Separator();
            ImVec4 color = feedbackIsError ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : ImVec4(0.3f, 1.0f, 0.3f, 1.0f);
            ImGui::TextColored(color, "%s", feedbackMessage.c_str());
        }
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Particles Alive: %d", (int)emitter->particles.size());
    }
}

void ComponentParticleSystem::SetTexture(const std::string& path) {
    if (!emitter) return;

    // Release previous resource
    if (textureResourceUID != 0) {
        Application::GetInstance().resources->ReleaseResource(textureResourceUID);
        textureResourceUID = 0;
        emitter->textureID = 0;
    }
    emitter->texturePath = path;
    if (path.empty()) return;

    // Search and Request new resource
    const auto& allResources = Application::GetInstance().resources->GetAllResources();
    for (const auto& pair : allResources) {
        Resource* res = pair.second;
        if (res->GetType() == Resource::TEXTURE && res->GetAssetFile() == path) {
            // Loaded form VRAM
            Resource* loadedRes = Application::GetInstance().resources->RequestResource(res->GetUID());
            if (loadedRes) {
                const ResourceTexture* texRes = static_cast<const ResourceTexture*>(loadedRes);
                emitter->textureID = texRes->GetGPU_ID();
                textureResourceUID = res->GetUID();
            }
            return;
        }
    }
}

// Json
void ComponentParticleSystem::Serialize(nlohmann::json& componentObj) const {
    componentObj["type"] = static_cast<int>(ComponentType::PARTICLE);
    componentObj["active"] = active;
    componentObj["maxParticles"] = emitter->maxParticles;
    componentObj["emissionRate"] = emitter->emissionRate;
    componentObj["texturePath"] = emitter->texturePath;

    componentObj["additive"] = emitter->additiveBlending;
    componentObj["textureRows"] = emitter->textureRows;
    componentObj["textureCols"] = emitter->textureCols;
    componentObj["animSpeed"] = emitter->animationSpeed;
    componentObj["animLoop"] = emitter->animLoop;

    for (auto mod : emitter->modules) {
        if (mod->type == ParticleModuleType::SPAWNER) {
            ModuleEmitterSpawn* s = static_cast<ModuleEmitterSpawn*>(mod);
            componentObj["shape"] = static_cast<int>(s->shape);
            componentObj["emissionArea"] = { s->emissionArea.x, s->emissionArea.y, s->emissionArea.z };
            componentObj["emissionRadius"] = s->emissionRadius;
            componentObj["coneAngle"] = s->coneAngle;

            componentObj["colorStart"] = { s->colorStart.r, s->colorStart.g, s->colorStart.b, s->colorStart.a };
            componentObj["colorEnd"] = { s->colorEnd.r, s->colorEnd.g, s->colorEnd.b, s->colorEnd.a };
            componentObj["sizeStart"] = s->sizeStart;
            componentObj["sizeEnd"] = s->sizeEnd;
            componentObj["rotSpeedMin"] = s->rotationSpeedMin;
            componentObj["rotSpeedMax"] = s->rotationSpeedMax;
            componentObj["lifetimeMin"] = s->lifetimeMin;
            componentObj["lifetimeMax"] = s->lifetimeMax;
            componentObj["speedMin"] = s->speedMin;
            componentObj["speedMax"] = s->speedMax;
        }
        else if (mod->type == ParticleModuleType::MOVEMENT) {
            ModuleEmitterMovement* m = static_cast<ModuleEmitterMovement*>(mod);
            componentObj["gravity"] = { m->gravity.x, m->gravity.y, m->gravity.z };
        }
    }
}

void ComponentParticleSystem::Deserialize(const nlohmann::json& componentObj) {
    if (componentObj.contains("active")) active = componentObj["active"];
    if (componentObj.contains("maxParticles")) emitter->maxParticles = componentObj["maxParticles"];
    if (componentObj.contains("emissionRate")) emitter->emissionRate = componentObj["emissionRate"];

    if (componentObj.contains("texturePath")) SetTexture(componentObj["texturePath"]);

    if (componentObj.contains("additive")) emitter->additiveBlending = componentObj["additive"];
    if (componentObj.contains("textureRows")) emitter->textureRows = componentObj["textureRows"];
    if (componentObj.contains("textureCols")) emitter->textureCols = componentObj["textureCols"];
    if (componentObj.contains("animSpeed")) emitter->animationSpeed = componentObj["animSpeed"];
    if (componentObj.contains("animLoop")) emitter->animLoop = componentObj["animLoop"];

    for (auto mod : emitter->modules) {
        if (mod->type == ParticleModuleType::SPAWNER) {
            ModuleEmitterSpawn* s = static_cast<ModuleEmitterSpawn*>(mod);
            if (componentObj.contains("shape")) s->shape = static_cast<EmitterShape>(componentObj["shape"]);
            if (componentObj.contains("emissionRadius")) s->emissionRadius = componentObj["emissionRadius"];
            if (componentObj.contains("coneAngle")) s->coneAngle = componentObj["coneAngle"];

            if (componentObj.contains("colorStart")) { auto c = componentObj["colorStart"]; s->colorStart = glm::vec4(c[0], c[1], c[2], c[3]); }
            if (componentObj.contains("colorEnd")) { auto c = componentObj["colorEnd"]; s->colorEnd = glm::vec4(c[0], c[1], c[2], c[3]); }
            if (componentObj.contains("sizeStart")) s->sizeStart = componentObj["sizeStart"];
            if (componentObj.contains("sizeEnd")) s->sizeEnd = componentObj["sizeEnd"];
            if (componentObj.contains("rotSpeedMin")) s->rotationSpeedMin = componentObj["rotSpeedMin"];
            if (componentObj.contains("rotSpeedMax")) s->rotationSpeedMax = componentObj["rotSpeedMax"];

            if (componentObj.contains("lifetimeMin")) s->lifetimeMin = componentObj["lifetimeMin"];
            if (componentObj.contains("lifetimeMax")) s->lifetimeMax = componentObj["lifetimeMax"];
            if (componentObj.contains("speedMin")) s->speedMin = componentObj["speedMin"];
            if (componentObj.contains("speedMax")) s->speedMax = componentObj["speedMax"];
            if (componentObj.contains("emissionArea")) { auto a = componentObj["emissionArea"]; s->emissionArea = glm::vec3(a[0], a[1], a[2]); }
        }
        else if (mod->type == ParticleModuleType::MOVEMENT) {
            ModuleEmitterMovement* m = static_cast<ModuleEmitterMovement*>(mod);
            if (componentObj.contains("gravity")) { auto g = componentObj["gravity"]; m->gravity = glm::vec3(g[0], g[1], g[2]); }
        }
    }
}

void ComponentParticleSystem::SaveParticleFile(const std::string& path) {
    std::filesystem::path filepath(path);
    std::filesystem::path dir = filepath.parent_path();
    // Check directory exists
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }

    nlohmann::json jsonFile;
    Serialize(jsonFile);
    std::ofstream file(path);
    if (file.is_open()) {
        file << std::setw(4) << jsonFile << std::endl;
        file.close();
        feedbackMessage = "Success: Saved " + filepath.filename().string();
        feedbackIsError = false;
        feedbackTimer = 3.0f;
        LOG_CONSOLE("Saved particle preset to %s", path.c_str());
    }
    else {
        feedbackMessage = "Error: Could not save file!";
        feedbackIsError = true;
        feedbackTimer = 3.0f;
        LOG_CONSOLE("ERROR: Could not save to %s", path.c_str());
    }
}

void ComponentParticleSystem::LoadParticleFile(const std::string& path) {
    std::ifstream file(path);
    if (file.is_open()) {
        nlohmann::json jsonFile;
        file >> jsonFile;
        Deserialize(jsonFile);
        file.close();
        std::filesystem::path filepath(path);
        feedbackMessage = "Success: Loaded " + filepath.filename().string();
        feedbackIsError = false;
        feedbackTimer = 3.0f;
        LOG_CONSOLE("Loaded particle preset from %s", path.c_str());
    }
    else {
        feedbackMessage = "Error: File not found!";
        feedbackIsError = true;
        feedbackTimer = 3.0f;
        LOG_CONSOLE("ERROR: Could not load %s", path.c_str());
    }
}