#include "CubemapEditorWindow.h"
#include "Application.h"
#include "ModuleResources.h"
#include "AssetsWindow.h"
#include "Cubemap.h"
#include "ResourceCubemap.h"
#include "CubemapImporter.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

CubemapEditorWindow::CubemapEditorWindow()
    : EditorWindow("Cubemap Editor")
{
    isOpen = false;
}

CubemapEditorWindow::~CubemapEditorWindow()
{
    if (editingCubemap) {
        Cubemap* cubemap = (Cubemap*)editingCubemap;
        cubemap->SetRightFaceTex(0);
        cubemap->SetLeftFaceTex(0);
        cubemap->SetTopFaceTex(0);
        cubemap->SetBottomFaceTex(0);
        cubemap->SetFrontFaceTex(0);
        cubemap->SetBackFaceTex(0);

        delete editingCubemap;
        editingCubemap = nullptr;
    }

    if (resCubemap) {
        Application::GetInstance().resources->ReleaseResource(currentCubemapUID);
        resCubemap = nullptr;
    }
}


void CubemapEditorWindow::Draw() {
    if (!isOpen) return;
    if (ImGui::Begin(name.c_str(), &isOpen)) {
        if (currentCubemapUID == 0 || !resCubemap || !editingCubemap) {
            ImGui::Text("Select a cubemap from Assets to edit.");
            ImGui::End();
            return;
        }

        Cubemap* cubemap = (Cubemap*)editingCubemap;

        UID rightTexUID = cubemap->GetRightFaceUID();
        if (DrawTextureSlot("Right Texture", rightTexUID, cubemap))
            cubemap->SetRightFaceTex(rightTexUID);
        UID leftTexUID = cubemap->GetLeftFaceUID();
        if (DrawTextureSlot("Left Texture", leftTexUID, cubemap))
            cubemap->SetLeftFaceTex(leftTexUID);
        UID topTexUID = cubemap->GetTopFaceUID();
        if (DrawTextureSlot("Top Texture", topTexUID, cubemap))
            cubemap->SetTopFaceTex(topTexUID);
        UID bottomTexUID = cubemap->GetBottomFaceUID();
        if (DrawTextureSlot("Bottome Texture", bottomTexUID, cubemap))
            cubemap->SetBottomFaceTex(bottomTexUID);
        UID frontTexUID = cubemap->GetFrontFaceUID();
        if (DrawTextureSlot("Front Texture", frontTexUID, cubemap))
            cubemap->SetFrontFaceTex(frontTexUID);
        UID backTexUID = cubemap->GetBackFaceUID();
        if (DrawTextureSlot("Back Texture", backTexUID, cubemap))
            cubemap->SetBackFaceTex(backTexUID);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Save Changes", ImVec2(120, 30))) {
            Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Discard", ImVec2(120, 30))) {
            SetCubemapToEdit(currentCubemapUID);
        }
    }
    ImGui::End();
}

void CubemapEditorWindow::SetCubemapToEdit(UID cubemapUID) {
    if (editingCubemap) {

        delete editingCubemap;
        editingCubemap = nullptr;
    }

    currentCubemapUID = cubemapUID;
    resCubemap = (ResourceCubemap*)Application::GetInstance().resources->RequestResource(currentCubemapUID);

    if (resCubemap && resCubemap->GetCubemap()) {

        editingCubemap = CubemapImporter::CloneCubemap(resCubemap->GetCubemap());
    }
}

bool CubemapEditorWindow::DrawTextureSlot(const char* label, UID& currentUID, Cubemap* cubemap) {
    bool changed = false;
    ImGui::Text(label);

    std::string buttonText = "None (Drop Texture)";
    if (currentUID != 0) {
        auto* res = Application::GetInstance().resources->PeekResource(currentUID);
        if (res) buttonText = std::to_string(res->GetUID());
    }

    ImGui::PushID(label);
    ImGui::Button(buttonText.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - 30, 20));

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
            const DragDropPayload* data = (const DragDropPayload*)payload->Data;

            if (data->assetType == DragDropAssetType::TEXTURE) {
                currentUID = data->assetUID;
                changed = true;
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();
    if (ImGui::Button("X")) {
        currentUID = 0;
        changed = true;
    }
    ImGui::PopID();

    return changed;
}

void CubemapEditorWindow::Save() {
    if (!resCubemap || !editingCubemap) return;

    nlohmann::json j;

    editingCubemap->SaveToJson(j);

    std::string path = resCubemap->GetAssetFile();
    std::ofstream file(path);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();

        Application::GetInstance().resources->ImportFile(path.c_str(), true);
        LOG_CONSOLE("[CubemapEditor] Saved and Re-imported: %s", path.c_str());
    }
}