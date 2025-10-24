#pragma once

#include "Component.h"
#include <string>
#include <memory>

class Texture;

class ComponentMaterial : public Component {
public:
    ComponentMaterial(GameObject* owner);
    ~ComponentMaterial();

    void Update() override;
    void OnEditor() override;

    bool LoadTexture(const std::string& path);
    void CreateCheckerboardTexture();
    void Use();
    void Unbind();
    bool HasTexture() const { return texture != nullptr; }

    const std::string& GetTexturePath() const { return texturePath; }
    int GetTextureWidth() const;
    int GetTextureHeight() const;

private:
    std::unique_ptr<Texture> texture;
    std::string texturePath;
};