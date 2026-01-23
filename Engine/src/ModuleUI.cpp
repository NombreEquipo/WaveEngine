#include "ModuleUI.h"
#include "Application.h"
#include "Input.h"
#include "Time.h"
#include "ModuleResources.h"
#include "ResourceTexture.h"
#include "MetaFile.h"
#include "imgui.h"
#include <glad/glad.h>

ModuleUI::ModuleUI() { name = "ModuleUI"; }

bool ModuleUI::Start() {

    UID bgUID = MetaFileManager::GetUIDFromAsset("Assets/Textures/menu_bg.png");
    if (bgUID != 0) {
        ResourceTexture* bgRes = static_cast<ResourceTexture*>(Application::GetInstance().resources->RequestResource(bgUID));
        if (bgRes) backgroundTextureID = bgRes->GetGPU_ID();
    }
    return true;
}

bool ModuleUI::Update() {
    float dt = Application::GetInstance().time->GetRealDeltaTime();

    if (currentState == UIState::MAIN_MENU) {

        if (Application::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && strlen(nameBuffer) > 0) {
            currentState = UIState::FADING;
        }
    }
    else if (currentState == UIState::FADING) {
        fadeAlpha += dt * fadeSpeed;
        if (fadeAlpha >= 1.0f) {
            fadeAlpha = 1.0f;
            currentState = UIState::GAME;
            Application::GetInstance().Play(); 
        }
    }
    return true;
}

bool ModuleUI::PostUpdate() {
    if (currentState == UIState::MAIN_MENU) DrawMainMenu();
    

    if (currentState == UIState::FADING || (currentState == UIState::GAME && fadeAlpha > 0.0f)) {
        DrawFade();
    }
    return true;
}

void ModuleUI::DrawMainMenu() {
    ImGuiIO& io = ImGui::GetIO();

    ImDrawList* drawList = ImGui::GetForegroundDrawList(); 


    if (backgroundTextureID != 0) {

        drawList->AddImage((ImTextureID)(uintptr_t)backgroundTextureID, ImVec2(0, 0), io.DisplaySize);
    } else {
 
        drawList->AddRectFilled(ImVec2(0, 0), io.DisplaySize, IM_COL32(255, 0, 0, 255));
    }

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    

    ImGui::Begin("Login", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("BIENVENIDO AL MOTOR");
    ImGui::Separator();
    ImGui::Spacing();


    ImGui::Text("Introduce tu nombre:");
    ImGui::InputText("##NameInput", nameBuffer, 64);

    ImGui::Spacing();


    if (ImGui::Button("START", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {

        if (strlen(nameBuffer) > 0) {
            currentState = UIState::FADING;
        }
    }

    ImGui::End();
}

void ModuleUI::DrawFade() {

    ImGui::GetForegroundDrawList()->AddRectFilled(
        ImVec2(0, 0), ImGui::GetIO().DisplaySize, 
        IM_COL32(0, 0, 0, (int)(fadeAlpha * 255))
    );
}