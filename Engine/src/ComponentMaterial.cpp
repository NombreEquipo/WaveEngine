#include "ComponentMaterial.h"
#include "GameObject.h"
#include "Texture.h"
#include <iostream>
#include "Log.h"

ComponentMaterial::ComponentMaterial(GameObject* owner)
    : Component(owner, ComponentType::MATERIAL),
    texture(nullptr),
    texturePath("")
{
    CreateCheckerboardTexture();
}

ComponentMaterial::~ComponentMaterial()
{

}

void ComponentMaterial::Update()
{

}

void ComponentMaterial::OnEditor()
{

}

bool ComponentMaterial::LoadTexture(const std::string& path)
{
    LOG("ComponentMaterial: Loading texture from %s", path);

    auto newTexture = std::make_unique<Texture>();

    if (newTexture->LoadFromFile(path))
    {
        texture = std::move(newTexture);
        texturePath = path;

        LOG("ComponentMaterial: Texture loaded");

        return true;
    }
    else
    {
        LOG("ComponentMaterial: Failed to load texture");
        return false;
    }
}

void ComponentMaterial::CreateCheckerboardTexture()
{

    LOG("ComponentMaterial: Checkerboard texture created");

    texture = std::make_unique<Texture>();

    texture->CreateCheckerboard();

    texturePath = "[Checkerboard Pattern]";
}

void ComponentMaterial::Use()
{
    if (texture)
    {
        texture->Bind();
    }
}

void ComponentMaterial::Unbind()
{
    if (texture)
    {
        texture->Unbind();
    }
}

int ComponentMaterial::GetTextureWidth() const
{
    if (texture)
    {
        return texture->GetWidth();
    }
    return 0;
}

int ComponentMaterial::GetTextureHeight() const
{
    if (texture)
    {
        return texture->GetHeight();
    }
    return 0;
}