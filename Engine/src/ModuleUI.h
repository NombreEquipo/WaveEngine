#pragma once
#include "Module.h"
#include <string>

enum class UIState {
    MAIN_MENU,
    FADING,
    GAME
};

class ModuleUI : public Module {
public:
    ModuleUI();
    ~ModuleUI() override = default;

    bool Start() override;
    bool Update() override;
    bool PostUpdate() override;

private:
    void DrawMainMenu();
    void DrawFade();

private:
    UIState currentState = UIState::MAIN_MENU;
    
    std::string userName;
    char nameBuffer[64] = ""; 

    float fadeAlpha = 0.0f;  
    float fadeSpeed = 1.0f;
    unsigned int backgroundTextureID = 0;
};