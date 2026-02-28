#pragma once

#include "NavMeshManager.h"
#include "ComponentNavigation.h"
#include "Application.h"
#include "imgui.h"
#include "GameObject.h"

ComponentNavigation::ComponentNavigation(GameObject* owner)
    : Component(owner, ComponentType::NAVIGATION)
{
}

void ComponentNavigation::OnEditor()
{
    if (ImGui::CollapsingHeader("Navigation & AI", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Navigation Static", &isStatic);

        //Select Type
        const char* items[] = { "Surface", "Agent", "Obstacle" };
     
        int currentType = static_cast<int>(type);
        if (ImGui::Combo("Nav Type", &currentType, items, IM_ARRAYSIZE(items)))
        {
            type = static_cast<NavType>(currentType);
        }

        ImGui::Spacing();

        if (type == NavType::SURFACE) {
            ImGui::Text("Surface Settings");
            ImGui::SliderFloat("Max Slope Angle", &maxSlopeAngle, 0.0f, 90.0f);
        }

        if (type == NavType::AGENT)
        {
            ImGui::Text("Agent Settings");
            ImGui::Separator();

            ImGui::Text("NavMesh Surface:");
            ImGui::SameLine();

            const char* surfaceName = linkedSurface ?
                linkedSurface->GetName().c_str() :
                "None (Drag Surface Here)";

            ImGui::Button(surfaceName, ImVec2(-1, 25));

            // --- Drag Target ---
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload =
                    ImGui::AcceptDragDropPayload("HIERARCHY_GAMEOBJECT"))
                {
                    GameObject* dropped =
                        *(GameObject**)payload->Data;

                    if (dropped)
                    {
                        ComponentNavigation* nav =
                            (ComponentNavigation*)dropped->GetComponent(ComponentType::NAVIGATION);

                        if (nav && nav->type == NavType::SURFACE)
                        {
                            linkedSurface = dropped;
                            LOG_CONSOLE("Agent linked to surface: %s", dropped->GetName().c_str());
                        }
                        else
                        {
                            LOG_CONSOLE("Dropped object is not a NavMesh Surface!");
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (linkedSurface)
            {
                if (ImGui::Button("Clear Surface"))
                {
                    linkedSurface = nullptr;
                }
            }
        }

        ImGui::Spacing();

        if (type != NavType::AGENT) {
            if (ImGui::Button("Bake NavMesh", ImVec2(-1, 30)))
            {
                Application::GetInstance().navMesh->Bake(this->owner);


            }

            ImGui::Spacing();

            if (ImGui::Button("Clear NavMesh", ImVec2(-1, 30)))
            {

                if (Application::GetInstance().navMesh)
                {
                    Application::GetInstance().navMesh->RemoveNavMesh(owner);
                }


            }
        }
     

    }
}