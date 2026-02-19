#pragma once
#include "GameObject.h"
#include "Component.h"
#include "UI.h"

class ComponentCanvas : public Component
{
public:
    ComponentCanvas(GameObject* owner);
    ~ComponentCanvas();

    void Update() override;

    void RenderToTexture();

    bool LoadXAML(const char* filename);
    void Resize(int width, int height);

    unsigned int GetTextureID() const { return textureID; }

private:
    void GenerateFramebuffer(int width, int height);

private:

    std::string currentXAML;
    Noesis::Ptr<Noesis::IView> view;
    unsigned int fbo = 0;
    unsigned int textureID = 0;
    unsigned int rbo = 0;

    int width = 1280;
    int height = 720;
};
    