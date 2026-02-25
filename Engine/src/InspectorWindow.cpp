#include "InspectorWindow.h"
#include "AssetsWindow.h"

#include <imgui.h>
#include "Application.h"
#include "GameObject.h"
#include "SelectionManager.h"
#include "Transform.h"
#include "ComponentMesh.h"
#include "ComponentSkinnedMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "CameraLens.h"
#include "ComponentRotate.h"
#include "ComponentAnimation.h"
#include "ResourceTexture.h"
#include "ComponentParticleSystem.h"
#include "Rigidbody.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"
#include "ConvexCollider.h"
#include "PlaneCollider.h"
#include "InfinitePlaneCollider.h"
#include "FixedJoint.h"
#include "HingeJoint.h"
#include "DistanceJoint.h"
#include "D6Joint.h"
#include "PrismaticJoint.h"
#include "SphericalJoint.h"
#include "AudioComponent.h"
#include "AudioSource.h"
#include "AudioListener.h"
#include "ReverbZone.h"

#include "Log.h"
#include "ComponentScript.h"
#include <filesystem>

InspectorWindow::InspectorWindow()
    : EditorWindow("Inspector")
{

  
}

void InspectorWindow::Draw()
{
    if (!isOpen) return;

    ImGui::Begin(name.c_str(), &isOpen);

    isHovered = (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_ChildWindows));

    SelectionManager* selectionManager = Application::GetInstance().selectionManager;

    if (!selectionManager->HasSelection())
    {
        ImGui::TextDisabled("No GameObject selected");
        ImGui::End();
        return;
    }

    GameObject* selectedObject = selectionManager->GetSelectedObject();

    if (selectedObject == nullptr || selectedObject->IsMarkedForDeletion())
    {
        ImGui::TextDisabled("Invalid selection");
        ImGui::End();
        return;
    }

    ImGui::Text("GameObject: %s", selectedObject->GetName().c_str());

    ImGui::Separator();

    bool objectDeleted = DrawGameObjectSection(selectedObject);
    if (objectDeleted)
    {
        ImGui::End();
        return;
    }

    //ImGui::Spacing();
    //if (ImGui::Button("Add Component", ImVec2(-1, 0))) {
    //    ImGui::OpenPopup("AddComponentPopup");
    //}

    //if (ImGui::BeginPopup("AddComponentPopup")) {
    //    
    //}
    //ImGui::Spacing();

    ImGui::Separator();
    DrawGizmoSettings();
    ImGui::Separator();

    for (Component* component : selectedObject->GetComponents())
    {
        switch (component->type)
        {
            // --- TRANSFORM & LÓGICA ---
        case ComponentType::TRANSFORM:
            DrawTransformComponent(component);
            break;
        case ComponentType::ROTATE:
            DrawRotateComponent(component);
            break;
        case ComponentType::SCRIPT:
            DrawScriptComponent(component);
            break;

            // --- GRÁFICOS & RENDER ---
        case ComponentType::SKINNED_MESH:
            DrawSkinnedMeshComponent(component);
            break;
        case ComponentType::MESH:
            DrawMeshComponent(component);
            break;
        
            break;
        case ComponentType::MATERIAL:
            DrawMaterialComponent(component);
            break;
        case ComponentType::CAMERA:
            DrawCameraComponent(component);
            break;
        case ComponentType::PARTICLE:
            DrawParticleComponent(component);
            break;
        case ComponentType::ANIMATION:
            DrawAnimationComponent(component);
            break;

            // --- FÍSICAS ---
        case ComponentType::RIGIDBODY:
            DrawRigidodyComponent(component);
            break;

            // --- FÍSICAS (Colliders) ---
        case ComponentType::BOX_COLLIDER:
            DrawBoxColliderComponent(component);
            break;
        case ComponentType::SPHERE_COLLIDER:
            DrawSphereColliderComponent(component);
            break;
        case ComponentType::CAPSULE_COLLIDER:
            DrawCapsuleColliderComponent(component);
            break;
        case ComponentType::PLANE_COLLIDER:
            DrawPlaneColliderComponent(component);
            break;
        case ComponentType::INFINITE_PLANE_COLLIDER:
            DrawInfinitePlaneColliderComponent(component);
            break;
        case ComponentType::MESH_COLLIDER:
            DrawMeshColliderComponent(component);
            break;
        case ComponentType::CONVEX_COLLIDER:
            DrawConvexColliderComponent(component);
            break;

            // --- FÍSICAS (Joints) ---
        case ComponentType::HINGE_JOINT:
            DrawHingeJointComponent(component);
            break;
        case ComponentType::DISTANCE_JOINT:
            DrawDistanceJointComponent(component);
            break;
        case ComponentType::FIXED_JOINT:
            DrawFixedJointComponent(component);
            break;
        case ComponentType::D6_JOINT:
            DrawD6JointComponent(component);
            break;
        case ComponentType::PRISMATIC_JOINT:
            DrawPrismaticJointComponent(component);
            break;
        case ComponentType::SPHERICAL_JOINT:
            DrawSphericalJointComponent(component);
            break;

            // --- AUDIO ---
        case ComponentType::LISTENER:
            DrawAudioListenerComponent(component);
            break;
        case ComponentType::AUDIOSOURCE:
            DrawAudioSourceComponent(component);
            break;
        case ComponentType::REVERBZONE:
            DrawReverbZoneComponent(component);
            break;

            // --- OTROS ---
        case ComponentType::UNKNOWN:
        default:
            LOG_DEBUG("WARNING: Trying to serialize an UNKNOWN or unhandled component type.");
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    DrawAddComponentButton(selectedObject);


    ImGui::End();
}

void InspectorWindow::DrawGizmoSettings()
{
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Gizmo Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Gizmo Mode:");
        ImGui::SameLine();
        ImGui::Text("Turn off (Q)");

        bool isTranslate = (currentGizmoOperation == ImGuizmo::TRANSLATE);
        bool isRotate = (currentGizmoOperation == ImGuizmo::ROTATE);
        bool isScale = (currentGizmoOperation == ImGuizmo::SCALE);

        if (ImGui::RadioButton("Translate (W)", isTranslate))
        {
            currentGizmoOperation = ImGuizmo::TRANSLATE;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Move the object in 3D space\nShortcut: W key");

        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate (E)", isRotate))
        {
            currentGizmoOperation = ImGuizmo::ROTATE;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Rotate the object\nShortcut: E key");

        ImGui::SameLine();
        if (ImGui::RadioButton("Scale (R)", isScale))
        {
            currentGizmoOperation = ImGuizmo::SCALE;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Scale the object\nShortcut: R key");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Transform Space (T):");
        ImGui::Spacing();

        bool isWorld = (currentGizmoMode == ImGuizmo::WORLD);
        bool isLocal = (currentGizmoMode == ImGuizmo::LOCAL);

        if (ImGui::RadioButton("World Space", isWorld))
        {
            currentGizmoMode = ImGuizmo::WORLD;
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "World Space");
            ImGui::Separator();
            ImGui::Text("Transformations are relative to world axes");
            ImGui::BulletText("X: Always points right");
            ImGui::BulletText("Y: Always points up");
            ImGui::BulletText("Z: Always points forward");
            ImGui::EndTooltip();
        }

        ImGui::SameLine();
        if (ImGui::RadioButton("Local Space", isLocal))
        {
            currentGizmoMode = ImGuizmo::LOCAL;
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Local Space");
            ImGui::Separator();
            ImGui::Text("Transformations are relative to objects rotation");
            ImGui::BulletText("Axes follow objects orientation");
            ImGui::BulletText("Useful for moving along objects direction");
            ImGui::EndTooltip();
        }
    }
}

void InspectorWindow::DrawTransformComponent(Component* component)
{
    Transform* transform = static_cast<Transform*>(component);

    if (transform == nullptr) return;

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        glm::vec3 position = transform->GetPosition();
        glm::vec3 rotation = transform->GetRotation();
        glm::vec3 scale = transform->GetScale();

        bool transformChanged = false;
        bool wasEditing = false;

        ImGui::Text("Position");
        ImGui::PushItemWidth(80);
        ImGui::Text("X"); ImGui::SameLine(20);
        if (ImGui::DragFloat("##PosX", &position.x, 0.1f)) transformChanged = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) wasEditing = true;

        ImGui::SameLine(120);
        ImGui::Text("Y"); ImGui::SameLine(130);
        if (ImGui::DragFloat("##PosY", &position.y, 0.1f)) transformChanged = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) wasEditing = true;

        ImGui::SameLine(230);
        ImGui::Text("Z"); ImGui::SameLine(240);
        if (ImGui::DragFloat("##PosZ", &position.z, 0.1f)) transformChanged = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) wasEditing = true;

        ImGui::Spacing();

        ImGui::Text("Rotation");
        ImGui::Text("X"); ImGui::SameLine(20);
        if (ImGui::DragFloat("##RotX", &rotation.x, 0.1f)) transformChanged = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) wasEditing = true;

        ImGui::SameLine(120);
        ImGui::Text("Y"); ImGui::SameLine(130);
        if (ImGui::DragFloat("##RotY", &rotation.y, 0.1f)) transformChanged = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) wasEditing = true;

        ImGui::SameLine(230);
        ImGui::Text("Z"); ImGui::SameLine(240);
        if (ImGui::DragFloat("##RotZ", &rotation.z, 0.1f)) transformChanged = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) wasEditing = true;

        ImGui::Spacing();

        ImGui::Text("Scale");
        ImGui::Text("X"); ImGui::SameLine(20);
        if (ImGui::DragFloat("##ScaleX", &scale.x, 0.1f)) transformChanged = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) wasEditing = true;

        ImGui::SameLine(120);
        ImGui::Text("Y"); ImGui::SameLine(130);
        if (ImGui::DragFloat("##ScaleY", &scale.y, 0.1f)) transformChanged = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) wasEditing = true;

        ImGui::SameLine(230);
        ImGui::Text("Z"); ImGui::SameLine(240);
        if (ImGui::DragFloat("##ScaleZ", &scale.z, 0.1f)) transformChanged = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) wasEditing = true;

        ImGui::PopItemWidth();

        if (transformChanged)
        {
            transform->SetPosition(position);
            transform->SetRotation(rotation);
            transform->SetScale(scale);
        }

        if (wasEditing)
        {
            Application::GetInstance().scene->MarkOctreeForRebuild();
            LOG_DEBUG("Octree rebuild requested after editing transform");
        }

        ImGui::Spacing();

        if (ImGui::Button("Reset Transform"))
        {
            transform->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
            transform->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
            transform->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

            // Rebuild después de reset
            Application::GetInstance().scene->MarkOctreeForRebuild();

            LOG_DEBUG("Transform reset for: %s", component->owner->GetName().c_str());
            LOG_CONSOLE("Transform reset for: %s", component->owner->GetName().c_str());
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Reset position to (0,0,0), rotation to (0,0,0), and scale to (1,1,1)");
        }
    }
}

void InspectorWindow::DrawCameraComponent(Component* component)
{
    ComponentCamera* cameraComp = (ComponentCamera*)component;

    if (cameraComp == nullptr) return;

    if (ImGui::CollapsingHeader("Camera"))
    {
        float fov = cameraComp->GetLens()->GetFov();
        if (ImGui::DragFloat("FOV", &fov, 0.1f, 1.0f, 160.0f))
        {
            cameraComp->GetLens()->SetFov(fov);
        }

        float zNear = cameraComp->GetLens()->GetNearPlane();
        if (ImGui::DragFloat("Near Plane", &zNear, 0.1f, 0.01f, 1000.0f))
        {
            cameraComp->GetLens()->SetNearPlane(zNear);
        }

        float zFar = cameraComp->GetLens()->GetFarPlane();
        if (ImGui::DragFloat("Far Plane", &zFar, 1.0f, 0.1f, 10000.0f))
        {
            cameraComp->GetLens()->SetFarPlane(zFar);
        }

        int depth = cameraComp->GetLens()->depth;
        if (ImGui::InputInt("Depth", &depth))
        {
            cameraComp->GetLens()->depth = glm::clamp(depth, 0, 100000);
        }

        bool isMain = cameraComp->IsMainCamera();
        if (ImGui::Checkbox("Is Main Camera", &isMain))
        {
            cameraComp->SetMainCamera(isMain);
        }

        ImGui::Text("FBO:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 0.7f, 0.9f, 1.0f), "%dx", cameraComp->GetLens()->fboID);

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        viewportSize.y = viewportSize.x / cameraComp->GetLens()->GetAspectRatio();
        ImVec2 winPos = ImGui::GetCursorScreenPos();
        unsigned int textureID = cameraComp->GetLens()->textureID;

        ImGui::Separator();
        ImGui::Image((ImTextureID)(intptr_t)textureID, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    }
}

void InspectorWindow::DrawMeshComponent(Component* component)
{
    ComponentMesh* meshComp = static_cast<ComponentMesh*>(component);
    if (meshComp == nullptr || meshComp->IsType(ComponentType::SKINNED_MESH)) return;

    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Mesh:");
        ImGui::SameLine();

        // Mesh display
        std::string currentMeshName = "None";
        if (meshComp->HasMesh() && meshComp->IsUsingResourceMesh())
        {
            UID currentUID = meshComp->GetMeshUID();
            ModuleResources* resources = Application::GetInstance().resources.get();
            const Resource* res = resources->GetResourceDirect(currentUID);
            if (res)
            {
                currentMeshName = std::string(res->GetAssetFile());
                // Filename
                size_t lastSlash = currentMeshName.find_last_of("/\\");
                if (lastSlash != std::string::npos)
                    currentMeshName = currentMeshName.substr(lastSlash + 1);
            }
            else
            {
                //Show UID
                currentMeshName = "UID " + std::to_string(currentUID);
            }
        }
        else if (meshComp->HasMesh() && meshComp->IsUsingDirectMesh())
        {
            currentMeshName = "[Primitive]";
        }

        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##MeshSelector", currentMeshName.c_str()))
        {
			// Get mesh resources
            ModuleResources* resources = Application::GetInstance().resources.get();
            const std::map<UID, Resource*>& allResources = resources->GetAllResources();

            for (const auto& pair : allResources)
            {
                const Resource* res = pair.second;
                if (res->GetType() == Resource::MESH)
                {
                    std::string meshName = res->GetAssetFile();

                    // Filename
                    size_t lastSlash = meshName.find_last_of("/\\");
                    if (lastSlash != std::string::npos) meshName = meshName.substr(lastSlash + 1);

                    UID meshUID = res->GetUID();
                    bool isSelected = (meshComp->IsUsingResourceMesh() && meshComp->GetMeshUID() == meshUID);

                    std::string displayName = meshName;
                    if (res->IsLoadedToMemory())
                    {
                        displayName += " [Loaded]";
                    }

                    if (ImGui::Selectable(displayName.c_str(), isSelected))
                    {
                        if (meshComp->LoadMeshByUID(meshUID))
                        {
                            LOG_DEBUG("Assigned mesh '%s' (UID %llu) to GameObject '%s'",
                                meshName.c_str(), meshUID, component->owner->GetName().c_str());
                            LOG_CONSOLE("Mesh '%s' assigned to '%s'",
                                meshName.c_str(), component->owner->GetName().c_str());
                        }
                        else
                        {
                            LOG_CONSOLE("Failed to load mesh '%s' (UID %llu)", meshName.c_str(), meshUID);
                        }
                    }

                    if (isSelected)
                    {
						ImGui::SetItemDefaultFocus(); // Highlight selected item
                    }

                    // Show tooltip with UID and path
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::Text("UID: %llu", meshUID);
                        ImGui::Text("Path: %s", res->GetAssetFile());
                        if (res->IsLoadedToMemory())
                        {
                            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Loaded in memory");
                        }
                        ImGui::EndTooltip();
                    }
                }
            }

            ImGui::EndCombo();
        }

        if (meshComp->HasMesh())
        {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            const Mesh& mesh = meshComp->GetMesh();

            ImGui::Text("Mesh Statistics:");
            ImGui::Text("Vertices: %d", (int)mesh.vertices.size());
            ImGui::Text("Indices: %d", (int)mesh.indices.size());
            ImGui::Text("Triangles: %d", (int)mesh.indices.size() / 3);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Debug Visualization:");
            bool showNormals = meshComp->GetDrawNormals();
            if (ImGui::Checkbox("Show Normals", &showNormals))
            {
                meshComp->SetDrawNormals(showNormals);
                LOG_DEBUG("Vertex normals visualization: %s", showNormals ? "ON" : "OFF");
            }

            bool showMesh = meshComp->GetDrawMesh();
            if (ImGui::Checkbox("Show Mesh", &showMesh))
            {
                meshComp->SetDrawMesh(showNormals);
                LOG_DEBUG("Face normals visualization: %s", showMesh ? "ON" : "OFF");
            }
        }
    }
}

void InspectorWindow::DrawSkinnedMeshComponent(Component* component)
{
    ComponentSkinnedMesh* meshComp = static_cast<ComponentSkinnedMesh*>(component);
    if (meshComp == nullptr) return;

    if (ImGui::CollapsingHeader("Skinned Mesh", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Mesh:");
        ImGui::SameLine();

        // Mesh display
        std::string currentMeshName = "None";
        if (meshComp->HasMesh() && meshComp->IsUsingResourceMesh())
        {
            UID currentUID = meshComp->GetMeshUID();
            ModuleResources* resources = Application::GetInstance().resources.get();
            const Resource* res = resources->GetResourceDirect(currentUID);
            if (res)
            {
                currentMeshName = std::string(res->GetAssetFile());
                // Filename
                size_t lastSlash = currentMeshName.find_last_of("/\\");
                if (lastSlash != std::string::npos)
                    currentMeshName = currentMeshName.substr(lastSlash + 1);
            }
            else
            {
                //Show UID
                currentMeshName = "UID " + std::to_string(currentUID);
            }
        }
        else if (meshComp->HasMesh() && meshComp->IsUsingDirectMesh())
        {
            currentMeshName = "[Primitive]";
        }

        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##MeshSelector", currentMeshName.c_str()))
        {
			// Get mesh resources
            ModuleResources* resources = Application::GetInstance().resources.get();
            const std::map<UID, Resource*>& allResources = resources->GetAllResources();

            for (const auto& pair : allResources)
            {
                const Resource* res = pair.second;
                if (res->GetType() == Resource::MESH)
                {
                    std::string meshName = res->GetAssetFile();

                    // Filename
                    size_t lastSlash = meshName.find_last_of("/\\");
                    if (lastSlash != std::string::npos) meshName = meshName.substr(lastSlash + 1);

                    UID meshUID = res->GetUID();
                    bool isSelected = (meshComp->IsUsingResourceMesh() && meshComp->GetMeshUID() == meshUID);

                    std::string displayName = meshName;
                    if (res->IsLoadedToMemory())
                    {
                        displayName += " [Loaded]";
                    }

                    if (ImGui::Selectable(displayName.c_str(), isSelected))
                    {
                        if (meshComp->LoadMeshByUID(meshUID))
                        {
                            LOG_DEBUG("Assigned mesh '%s' (UID %llu) to GameObject '%s'",
                                meshName.c_str(), meshUID, component->owner->GetName().c_str());
                            LOG_CONSOLE("Mesh '%s' assigned to '%s'",
                                meshName.c_str(), component->owner->GetName().c_str());
                        }
                        else
                        {
                            LOG_CONSOLE("Failed to load mesh '%s' (UID %llu)", meshName.c_str(), meshUID);
                        }
                    }

                    if (isSelected)
                    {
						ImGui::SetItemDefaultFocus(); // Highlight selected item
                    }

                    // Show tooltip with UID and path
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::Text("UID: %llu", meshUID);
                        ImGui::Text("Path: %s", res->GetAssetFile());
                        if (res->IsLoadedToMemory())
                        {
                            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Loaded in memory");
                        }
                        ImGui::EndTooltip();
                    }
                }
            }

            ImGui::EndCombo();
        }

        if (meshComp->HasMesh())
        {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            const Mesh& mesh = meshComp->GetMesh();

            ImGui::Text("Mesh Statistics:");
            ImGui::Text("Vertices: %d", (int)mesh.vertices.size());
            ImGui::Text("Indices: %d", (int)mesh.indices.size());
            ImGui::Text("Triangles: %d", (int)mesh.indices.size() / 3);
            ImGui::Text("Linked bones: %d / %d", meshComp->GetLinkedBonesNum(), (int)mesh.bones.size());

            if (ImGui::Button("Link Bones"))
            {
                meshComp->LinkBones();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Debug Visualization:");
            bool showNormals = meshComp->GetDrawNormals();
            if (ImGui::Checkbox("Show Normals", &showNormals))
            {
                meshComp->SetDrawNormals(showNormals);
                LOG_DEBUG("Vertex normals visualization: %s", showNormals ? "ON" : "OFF");
            }

            bool showMesh = meshComp->GetDrawMesh();
            if (ImGui::Checkbox("Show Mesh", &showMesh))
            {
                meshComp->SetDrawMesh(showMesh);
                LOG_DEBUG("Face normals visualization: %s", showMesh ? "ON" : "OFF");
            }
        }
    }
}

void InspectorWindow::DrawMaterialComponent(Component* component)
{
    ComponentMaterial* materialComp = static_cast<ComponentMaterial*>(component);

    if (materialComp == nullptr) return;

    materialComp->OnEditor();
}

void InspectorWindow::DrawRotateComponent(Component* component)
{
    ComponentRotate* rotateComp = static_cast<ComponentRotate*>(component);

    if (rotateComp == nullptr) return;

    if (ImGui::CollapsingHeader("Auto Rotate", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool active = rotateComp->IsActive();
        if (ImGui::Checkbox("Enable Auto Rotation", &active))
        {
            rotateComp->SetActive(active);
        }

        rotateComp->OnEditor();
    }
}

void InspectorWindow::DrawParticleComponent(Component* component)
{
    ComponentParticleSystem* particleComp = static_cast<ComponentParticleSystem*>(component);

    if (particleComp != nullptr)
    {
        // Delegate the ui to the component
        particleComp->OnEditor();
    }
}

void  InspectorWindow::DrawRigidodyComponent(Component* component)
{
    Rigidbody* rigidbody = static_cast<Rigidbody*>(component);

    if (rigidbody != nullptr)
    {
        if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            rigidbody->OnEditor();
        }
    }
}

void  InspectorWindow::DrawBoxColliderComponent(Component* component)
{
    BoxCollider* boxCollider = static_cast<BoxCollider*>(component);

    if (boxCollider != nullptr)
    {
        if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            boxCollider->OnEditor();
        }
    }
}

void  InspectorWindow::DrawSphereColliderComponent(Component* component)
{
    SphereCollider* Collider = static_cast<SphereCollider*>(component);

    if (Collider != nullptr)
    {
        if (ImGui::CollapsingHeader("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Collider->OnEditor();
        }
    }
}

void  InspectorWindow::DrawCapsuleColliderComponent(Component* component)
{
    CapsuleCollider* Collider = static_cast<CapsuleCollider*>(component);

    if (Collider != nullptr)
    {
        if (ImGui::CollapsingHeader("Capsule Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Collider->OnEditor();
        }
        
    }
}

void  InspectorWindow::DrawPlaneColliderComponent(Component* component)
{
    PlaneCollider* Collider = static_cast<PlaneCollider*>(component);

    if (Collider != nullptr)
    {
        if (ImGui::CollapsingHeader("Plane Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Collider->OnEditor();
        }
    }
}
void  InspectorWindow::DrawInfinitePlaneColliderComponent(Component* component)
{
    InfinitePlaneCollider* Collider = static_cast<InfinitePlaneCollider*>(component);

    if (Collider != nullptr)
    {
        if (ImGui::CollapsingHeader("Infinite Plane Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Collider->OnEditor();
        }
        
    }
}
void  InspectorWindow::DrawMeshColliderComponent(Component* component)
{
    MeshCollider* Collider = static_cast<MeshCollider*>(component);

    if (Collider != nullptr)
    {
        if (ImGui::CollapsingHeader("Mesh Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Collider->OnEditor();
        }
    }
}
void  InspectorWindow::DrawConvexColliderComponent(Component* component)
{
    ConvexCollider* Collider = static_cast<ConvexCollider*>(component);

    if (Collider != nullptr)
    {
        if (ImGui::CollapsingHeader("Convex Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Collider->OnEditor();
        }
    }
}

void  InspectorWindow::DrawFixedJointComponent(Component* component)
{
    FixedJoint* Joint = static_cast<FixedJoint*>(component);

    if (Joint != nullptr)
    {
        if (ImGui::CollapsingHeader("Fixed Joint", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Joint->OnEditor();
        }
    }
}

void  InspectorWindow::DrawDistanceJointComponent(Component* component)
{
    DistanceJoint* Joint = static_cast<DistanceJoint*>(component);

    if (Joint != nullptr)
    {
        if (ImGui::CollapsingHeader("Distance Joint", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Joint->OnEditor();
        }
    }
}

void  InspectorWindow::DrawHingeJointComponent(Component* component)
{
    HingeJoint* Joint = static_cast<HingeJoint*>(component);

    if (Joint != nullptr)
    {
        if (ImGui::CollapsingHeader("Hinge Joint", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Joint->OnEditor();
        }
    }
}

void  InspectorWindow::DrawPrismaticJointComponent(Component* component)
{
    PrismaticJoint* Joint = static_cast<PrismaticJoint*>(component);

    if (Joint != nullptr)
    {
        if (ImGui::CollapsingHeader("Prismatic Joint", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Joint->OnEditor();
        }
    }
}

void  InspectorWindow::DrawSphericalJointComponent(Component* component)
{
    SphericalJoint* Joint = static_cast<SphericalJoint*>(component);

    if (Joint != nullptr)
    {
        if (ImGui::CollapsingHeader("Spherical Joint", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Joint->OnEditor();
        }
    }
}

void  InspectorWindow::DrawD6JointComponent(Component* component)
{
    D6Joint* Joint = static_cast<D6Joint*>(component);

    if (Joint != nullptr)
    {
        if (ImGui::CollapsingHeader("D6 Joint", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Delegate the ui to the component
            Joint->OnEditor();
        }
    }
}


void InspectorWindow::DrawAudioSourceComponent(Component* component) {
    AudioSource* source = static_cast<AudioSource*>(component);
    if (!source) return;

    if (ImGui::CollapsingHeader("Audio Source", ImGuiTreeNodeFlags_DefaultOpen)) {
        source->OnEditor();
    }
}

void InspectorWindow::DrawAudioListenerComponent(Component* component) {
    AudioListener* listener = static_cast<AudioListener*>(component);
    if (!listener) return;

    if (ImGui::CollapsingHeader("Audio Listener", ImGuiTreeNodeFlags_DefaultOpen)) {
        listener->OnEditor();
    }
}

void InspectorWindow::DrawReverbZoneComponent(Component* component)
{
    ReverbZone* zone = static_cast<ReverbZone*>(component);
    if (!zone) return;

    if (ImGui::CollapsingHeader("Reverb Zone", ImGuiTreeNodeFlags_DefaultOpen)) {
        zone->OnEditor();
    }
}

void InspectorWindow::DrawAnimationComponent(Component* component)
{
    ComponentAnimation* animation = static_cast<ComponentAnimation*>(component);
    if (!animation) return;

    if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Separator();
        ImGui::Text("Library:");

        int i = 0;
        for (auto it = animation->animationsLibrary.begin(); it != animation->animationsLibrary.end(); )
        {
            ImGui::PushID(it->first.c_str());

            bool deleteRequested = false;

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;

            bool isNodeOpen = ImGui::TreeNodeEx(it->first.c_str(), flags);

            ImGui::SameLine();

            float buttonWidth = 20.0f;
            float availableWidth = ImGui::GetContentRegionAvail().x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availableWidth - buttonWidth);

            if (ImGui::SmallButton("X"))
            {
                deleteRequested = true;
            }

            if (isNodeOpen)
            {
                if (!deleteRequested)
                {
                    ImGui::Unindent();
                    ImGui::Text("UID: %d", it->second.uid);


                    ImGui::Text("Speed");
                    ImGui::SameLine();
                    float speed = it->second.speed;
                    if (ImGui::InputFloat("##Speed", &speed))
                    {
                        animation->SetAnimationSpeed(it->first, speed);
                    }

                    ImGui::Text("Loop");
                    ImGui::SameLine();
                    bool loop = it->second.loop;
                    if (ImGui::Checkbox("##Loop", &loop))
                    {
                        animation->SetAnimationLoop(it->first, loop);
                    }

                    if (ImGui::Button("Play", ImVec2(-1, 0)))
                    {
                        animation->Play(it->first, 0.5f);
                    }
                    ImGui::Indent();
                }

                ImGui::TreePop();
            }

            ImGui::PopID();

            if (deleteRequested)
            {
                auto nextIt = it;
                ++nextIt;

                animation->RemoveAnimation(it->first);

                it = nextIt;
            }
            else
            {
                ++it;
            }
            i++;
        }

        if (i == 0)
        {
            ImGui::SameLine();
            ImGui::Text("empty");
        }

        ImGui::Separator();

        static char nameBuffer[64] = "New Animation";
        int availableWidth = ImGui::GetContentRegionAvail().x;

        ImGui::InputText(" ", nameBuffer, 64);
        ImGui::Button("Drop animation", ImVec2(availableWidth, 20));
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_ITEM"))
            {
                DragDropPayload* dropData = (DragDropPayload*)payload->Data;
                UID droppedUID = dropData->assetUID;

                const Resource* res = Application::GetInstance().resources->PeekResource(droppedUID);
                if (res && res->GetType() == Resource::Type::ANIMATION)
                {
                    animation->AddAnimation(nameBuffer, droppedUID);
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
}


bool InspectorWindow::DrawGameObjectSection(GameObject* selectedObject)
{
    bool objectDeleted = false;

    if (ImGui::CollapsingHeader("GameObject", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Actions:");
        ImGui::Spacing();

        // Delete button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));

        if (ImGui::Button("Delete GameObject", ImVec2(-1, 0)))
        {
            if (selectedObject != Application::GetInstance().scene->GetRoot())
            {
                selectedObject->MarkForDeletion();
                LOG_DEBUG("GameObject '%s' marked for deletion", selectedObject->GetName().c_str());
                LOG_CONSOLE("GameObject '%s' marked for deletion", selectedObject->GetName().c_str());

                Application::GetInstance().selectionManager->ClearSelection();
                objectDeleted = true;
            }
            else
            {
                LOG_CONSOLE("Cannot delete Root GameObject!");
            }
        }

        ImGui::PopStyleColor(3);

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Delete GameObject");
            ImGui::Separator();
            ImGui::Text("Marks this GameObject for deletion");
            ImGui::Text("Shortcut: Backspace key");
            ImGui::EndTooltip();
        }

        ImGui::Spacing();

        // Create empty child button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));

        if (ImGui::Button("Create Empty Child", ImVec2(-1, 0)))
        {
            GameObject* newChild = Application::GetInstance().scene->CreateGameObject("Empty");
            newChild->SetParent(selectedObject);

            Application::GetInstance().selectionManager->SetSelectedObject(newChild);

            LOG_DEBUG("Created empty child for '%s'", selectedObject->GetName().c_str());
            LOG_CONSOLE("Created empty child '%s' under '%s'", newChild->GetName().c_str(), selectedObject->GetName().c_str());
        }

        ImGui::PopStyleColor(3);

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Create Empty Child");
            ImGui::Separator();
            ImGui::Text("Creates a new empty GameObject as a child");
            ImGui::Text("of this GameObject");
            ImGui::EndTooltip();
        }
    }

    return objectDeleted;
}


void InspectorWindow::GetAllGameObjects(GameObject* root, std::vector<GameObject*>& outObjects)
{
    if (root == nullptr)
        return;

    outObjects.push_back(root);

    const std::vector<GameObject*>& children = root->GetChildren();
    for (GameObject* child : children)
    {
        GetAllGameObjects(child, outObjects);
    }
}

bool InspectorWindow::IsDescendantOf(GameObject* potentialDescendant, GameObject* potentialAncestor)
{
    if (potentialDescendant == nullptr || potentialAncestor == nullptr)
        return false;

    GameObject* current = potentialDescendant->GetParent();
    while (current != nullptr)
    {
        if (current == potentialAncestor)
            return true;
        current = current->GetParent();
    }

    return false;
}

void InspectorWindow::DrawScriptComponent(Component* component)
{
    ComponentScript* scriptComp = static_cast<ComponentScript*>(
        component
        );

    if (scriptComp == nullptr) return;

    if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent();

        if (scriptComp->HasScript()) {
            ModuleResources* resources = Application::GetInstance().resources.get();
            const Resource* res = resources->GetResourceDirect(scriptComp->GetScriptUID());

            if (res) {
                std::string filename = std::filesystem::path(res->GetAssetFile()).filename().string();

                ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Current Script:");
                ImGui::SameLine();
                ImGui::Text("%s", filename.c_str());

                ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "UID: %llu", scriptComp->GetScriptUID());

                ImGui::Spacing();

                if (ImGui::Button("Change Script", ImVec2(120, 0))) {
                    ImGui::OpenPopup("SelectScript");
                }

                ImGui::SameLine();

                if (ImGui::Button("Remove Script", ImVec2(120, 0))) {
                    scriptComp->UnloadScript();
                    LOG_CONSOLE("[Inspector] Script removed from: %s", component->owner->GetName().c_str());
                }

                ImGui::SameLine();

                if (ImGui::Button("Reload", ImVec2(80, 0))) {
                    scriptComp->ReloadScript();
                    LOG_CONSOLE("[Inspector] Script reloaded for: %s", component->owner->GetName().c_str());
                }

                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Hot reload the script");
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                const auto& publicVars = scriptComp->GetPublicVariables();

                if (!publicVars.empty()) {
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Public Variables");
                    ImGui::Spacing();

                    for (size_t i = 0; i < publicVars.size(); ++i) {
                        const ScriptVariable& var = publicVars[i];

                        ImGui::PushID(i);

                        switch (var.type) {
                        case ScriptVarType::NUMBER: {
                            float value = std::get<float>(var.value);
                            if (ImGui::DragFloat(var.name.c_str(), &value, 0.1f)) {
                                ScriptVariable newVar(var.name, value);
                                scriptComp->UpdatePublicVariable(i, newVar);
                            }
                            break;
                        }

                        case ScriptVarType::STRING: {
                            static char buffer[256];
                            std::string value = std::get<std::string>(var.value);
                            strncpy(buffer, value.c_str(), sizeof(buffer) - 1);

                            if (ImGui::InputText(var.name.c_str(), buffer, sizeof(buffer))) {
                                ScriptVariable newVar(var.name, std::string(buffer));
                                scriptComp->UpdatePublicVariable(i, newVar);
                            }
                            break;
                        }

                        case ScriptVarType::BOOLEAN: {
                            bool value = std::get<bool>(var.value);
                            if (ImGui::Checkbox(var.name.c_str(), &value)) {
                                ScriptVariable newVar(var.name, value);
                                scriptComp->UpdatePublicVariable(i, newVar);
                            }
                            break;
                        }

                        case ScriptVarType::VEC3: {
                            glm::vec3 value = std::get<glm::vec3>(var.value);

                            ImGui::Text("%s", var.name.c_str());
                            ImGui::PushItemWidth(80);

                            bool changed = false;
                            ImGui::Text("X"); ImGui::SameLine(20);
                            if (ImGui::DragFloat("##X", &value.x, 0.1f)) changed = true;

                            ImGui::SameLine(120);
                            ImGui::Text("Y"); ImGui::SameLine(130);
                            if (ImGui::DragFloat("##Y", &value.y, 0.1f)) changed = true;

                            ImGui::SameLine(230);
                            ImGui::Text("Z"); ImGui::SameLine(240);
                            if (ImGui::DragFloat("##Z", &value.z, 0.1f)) changed = true;

                            ImGui::PopItemWidth();

                            if (changed) {
                                ScriptVariable newVar(var.name, value);
                                scriptComp->UpdatePublicVariable(i, newVar);
                            }
                            break;
                        }
                        }

                        ImGui::PopID();
                    }
                }
                else {
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                        "No public variables defined");
                    ImGui::Spacing();
                    ImGui::TextWrapped("Define variables in a 'public' table in your Lua script");
                }
            }
        }
        else {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "No script assigned");

            if (ImGui::Button("Assign Script", ImVec2(150, 0))) {
                ImGui::OpenPopup("SelectScript");
            }
        }

        // Popup para seleccionar script
        if (ImGui::BeginPopup("SelectScript")) {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Select Lua Script");
            ImGui::Separator();

            ModuleResources* resources = Application::GetInstance().resources.get();
            const auto& allResources = resources->GetAllResources();

            bool foundScripts = false;

            for (const auto& [uid, resource] : allResources) {
                if (resource->GetType() == Resource::SCRIPT) {
                    foundScripts = true;

                    std::string filepath = resource->GetAssetFile();
                    std::string filename = std::filesystem::path(filepath).filename().string();

                    bool isSelected = (scriptComp->HasScript() && scriptComp->GetScriptUID() == uid);

                    if (ImGui::Selectable(filename.c_str(), isSelected)) {
                        if (scriptComp->LoadScriptByUID(uid)) {
                            LOG_CONSOLE("[Inspector] Script '%s' assigned to '%s'",
                                filename.c_str(), component->owner->GetName().c_str());
                        }
                        else {
                            LOG_CONSOLE("[Inspector] Failed to load script '%s'", filename.c_str());
                        }
                        ImGui::CloseCurrentPopup();
                    }

                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }

                    if (ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        ImGui::Text("UID: %llu", uid);
                        ImGui::Text("Path: %s", filepath.c_str());
                        ImGui::EndTooltip();
                    }
                }
            }

            if (!foundScripts) {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "No .lua scripts found");
                ImGui::Spacing();
                ImGui::TextWrapped("Create a .lua file in Assets/Scripts/");
            }

            ImGui::EndPopup();
        }

        ImGui::Unindent();
    }
}

void InspectorWindow::DrawAddComponentButton(GameObject* selectedObject)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.4f, 0.7f, 1.0f));

    if (ImGui::Button("Add Component", ImVec2(-1, 30)))
    {
        ImGui::OpenPopup("AddComponentPopup");
    }

    ImGui::PopStyleColor(3);

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Add Component");
        ImGui::Separator();
        ImGui::Text("Add a new component to this GameObject");
        ImGui::EndTooltip();
    }

    // Popup con lista de componentes
    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Select Component Type");
        ImGui::Separator();
        ImGui::Spacing();

        // Script Component
        bool hasScript = (selectedObject->GetComponent(ComponentType::SCRIPT) != nullptr);

        if (hasScript)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }

        if (ImGui::Selectable("Script", false, hasScript ? ImGuiSelectableFlags_Disabled : 0))
        {
            selectedObject->CreateComponent(ComponentType::SCRIPT);  

            LOG_CONSOLE("[Inspector] Script component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (hasScript)
        {
            ImGui::PopStyleColor();
        }

        if (ImGui::IsItemHovered() && !hasScript)
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Lua script to this GameObject");
            ImGui::EndTooltip();
        }
        else if (ImGui::IsItemHovered() && hasScript)
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Already has a Script component");
            ImGui::EndTooltip();
        }

        // Camera Component
        bool hasCamera = (selectedObject->GetComponent(ComponentType::CAMERA) != nullptr);

        if (hasCamera)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }

        if (ImGui::Selectable("Camera", false, hasCamera ? ImGuiSelectableFlags_Disabled : 0))
        {
            selectedObject->CreateComponent(ComponentType::CAMERA);  

            LOG_CONSOLE("[Inspector] Camera component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (hasCamera)
        {
            ImGui::PopStyleColor();
        }

        if (ImGui::IsItemHovered() && !hasCamera)
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Camera to this GameObject");
            ImGui::EndTooltip();
        }
        else if (ImGui::IsItemHovered() && hasCamera)
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Already has a Camera component");
            ImGui::EndTooltip();
        }

        // Mesh Component
        bool hasMesh = (selectedObject->GetComponent(ComponentType::MESH) != nullptr);

        if (hasMesh)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }

        if (ImGui::Selectable("Mesh Renderer", false, hasMesh ? ImGuiSelectableFlags_Disabled : 0))
        {
            selectedObject->CreateComponent(ComponentType::MESH);  

            LOG_CONSOLE("[Inspector] Mesh component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (hasMesh)
        {
            ImGui::PopStyleColor();
        }

        if (ImGui::IsItemHovered() && !hasMesh)
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Mesh Renderer to this GameObject");
            ImGui::EndTooltip();
        }
        else if (ImGui::IsItemHovered() && hasMesh)
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Already has a Mesh component");
            ImGui::EndTooltip();
        }

        // Material Component
        bool hasMaterial = (selectedObject->GetComponent(ComponentType::MATERIAL) != nullptr);

        if (hasMaterial)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }

        if (ImGui::Selectable("Material", false, hasMaterial ? ImGuiSelectableFlags_Disabled : 0))
        {
            selectedObject->CreateComponent(ComponentType::MATERIAL); 

            LOG_CONSOLE("[Inspector] Material component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (hasMaterial)
        {
            ImGui::PopStyleColor();
        }

        if (ImGui::IsItemHovered() && !hasMaterial)
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Material to this GameObject");
            ImGui::EndTooltip();
        }
        else if (ImGui::IsItemHovered() && hasMaterial)
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Already has a Material component");
            ImGui::EndTooltip();
        }


        // Particle Component
        bool hasParticles = (selectedObject->GetComponent(ComponentType::PARTICLE) != nullptr);

        if (hasParticles) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        if (ImGui::Selectable("Particle System", false, hasParticles ? ImGuiSelectableFlags_Disabled : 0))
        {
            selectedObject->CreateComponent(ComponentType::PARTICLE);
            LOG_CONSOLE("[Inspector] Particle System component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (hasParticles) ImGui::PopStyleColor();

        if (ImGui::IsItemHovered() && !hasParticles)
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Particle System");
            ImGui::EndTooltip();
        }
        else if (ImGui::IsItemHovered() && hasParticles)
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Already has a Particle System");
            ImGui::EndTooltip();
        }

        bool hasRigidbody = (selectedObject->GetComponent(ComponentType::RIGIDBODY) != nullptr);
        
        if (hasRigidbody) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        if (ImGui::Selectable("Rigidbody", false, hasParticles ? ImGuiSelectableFlags_Disabled : 0))
        {
            selectedObject->CreateComponent(ComponentType::RIGIDBODY);
            LOG_CONSOLE("[Inspector] Rigidbody component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (hasRigidbody) ImGui::PopStyleColor();

        if (ImGui::IsItemHovered() && !hasRigidbody)
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Rigidbody");
            ImGui::EndTooltip();
        }
        else if (ImGui::IsItemHovered() && hasRigidbody)
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Already has a Rigidbody");
            ImGui::EndTooltip();
        }


        if (ImGui::Selectable("Box Collider", false))
        {
            selectedObject->CreateComponent(ComponentType::BOX_COLLIDER);
            LOG_CONSOLE("[Inspector] Box Collider component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Box Collider");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("Sphere Collider", false))
        {
            selectedObject->CreateComponent(ComponentType::SPHERE_COLLIDER);
            LOG_CONSOLE("[Inspector] Sphere Collider component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Sphere Collider");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("Capsule Collider", false))
        {
            selectedObject->CreateComponent(ComponentType::CAPSULE_COLLIDER);
            LOG_CONSOLE("[Inspector] Capsule Collider component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Capsule Collider");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("Plane Collider", false))
        {
            selectedObject->CreateComponent(ComponentType::PLANE_COLLIDER);
            LOG_CONSOLE("[Inspector] Plane Collider component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Plane Collider");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("Infinite Plane Collider", false))
        {
            selectedObject->CreateComponent(ComponentType::INFINITE_PLANE_COLLIDER);
            LOG_CONSOLE("[Inspector] Infinite Plane Collider component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Infinite Plane Collider");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("Mesh Collider", false))
        {
            selectedObject->CreateComponent(ComponentType::MESH_COLLIDER);
            LOG_CONSOLE("[Inspector] Mesh Collider component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Mesh Collider");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("Convex Collider", false))
        {
            selectedObject->CreateComponent(ComponentType::CONVEX_COLLIDER);
            LOG_CONSOLE("[Inspector] Convex Collider component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Convex Collider");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("Fixed Joint", false))
        {
            selectedObject->CreateComponent(ComponentType::FIXED_JOINT);
            LOG_CONSOLE("[Inspector] Fixed Joint component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Fixed Joint");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("Distance Joint", false))
        {
            selectedObject->CreateComponent(ComponentType::DISTANCE_JOINT);
            LOG_CONSOLE("[Inspector] Distance Joint component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Distance Joint");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("Hinge Joint", false))
        {
            selectedObject->CreateComponent(ComponentType::HINGE_JOINT);
            LOG_CONSOLE("[Inspector] Hinge Joint component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Hinge Joint");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("Spherical Joint", false))
        {
            selectedObject->CreateComponent(ComponentType::SPHERICAL_JOINT);
            LOG_CONSOLE("[Inspector] Spherical Joint component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Spherical Joint");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("Prismatic Joint", false))
        {
            selectedObject->CreateComponent(ComponentType::PRISMATIC_JOINT);
            LOG_CONSOLE("[Inspector] Prismatic Joint component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Prismatic Joint");
            ImGui::EndTooltip();
        }
        
        if (ImGui::Selectable("D6 Joint", false))
        {
            selectedObject->CreateComponent(ComponentType::D6_JOINT);
            LOG_CONSOLE("[Inspector] D6 Joint component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a D6 Joint");
            ImGui::EndTooltip();
        }
        //Audio Source Component --> 0 to multiple per object
        if (ImGui::Selectable("Audio Source"))
        {
            selectedObject->CreateComponent(ComponentType::AUDIOSOURCE);

            LOG_CONSOLE("[Inspector] AudioSource component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup(); // disappear automatically after selecting an option
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add an Audio Source");
            ImGui::EndTooltip();
        }

        //Audio Listener Component --> 0 to max 1 per object
        bool hasListener = (selectedObject->GetComponent(ComponentType::LISTENER) != nullptr);

        if (hasListener) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        if (ImGui::Selectable("Audio Listener", false, hasListener ? ImGuiSelectableFlags_Disabled : 0))
        {
            selectedObject->CreateComponent(ComponentType::LISTENER);
            LOG_CONSOLE("[Inspector] Audio Listener component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (hasListener) ImGui::PopStyleColor();

        if (ImGui::IsItemHovered() && !hasListener)
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add an Audio Listener");
            ImGui::EndTooltip();
        }
        else if (ImGui::IsItemHovered() && hasListener)
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Already has an Audio Listener");
            ImGui::EndTooltip();
        }

        //Reverb Zone Component
        if (ImGui::Selectable("Reverb Zone")) {
            selectedObject->CreateComponent(ComponentType::REVERBZONE);
            LOG_CONSOLE("[Inspector] ReverbZone component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add a Reverb Zone");
            ImGui::EndTooltip();
        }

        bool hasAnimation = (selectedObject->GetComponent(ComponentType::ANIMATION) != nullptr);

        if (hasAnimation) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        if (ImGui::Selectable("Animation", false, hasAnimation ? ImGuiSelectableFlags_Disabled : 0))
        {
            selectedObject->CreateComponent(ComponentType::ANIMATION);
            LOG_CONSOLE("[Inspector] Animation component added to: %s", selectedObject->GetName().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (hasAnimation) ImGui::PopStyleColor();

        if (ImGui::IsItemHovered() && !hasAnimation)
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add an Animation");
            ImGui::EndTooltip();
        }
        else if (ImGui::IsItemHovered() && hasAnimation)
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Already has an Animation");
            ImGui::EndTooltip();
        }

        ImGui::EndPopup();
    }
}