#include "ComponentMaterial.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ResourceTexture.h"
#include "Log.h"
#include <glad/glad.h>

ComponentMaterial::ComponentMaterial(GameObject* owner)
    : Component(owner, ComponentType::MATERIAL),
    textureUID(0),
    originalTextureUID(0),
    texturePath(""),
    originalTexturePath("")
{
    LOG_CONSOLE("[ComponentMaterial] Created for GameObject: %s",
        owner ? owner->GetName().c_str() : "NULL");
}

ComponentMaterial::~ComponentMaterial()
{
    LOG_CONSOLE("[ComponentMaterial] Destroying component for GameObject: %s",
        owner ? owner->GetName().c_str() : "NULL");

    ReleaseCurrentTexture();
}

void ComponentMaterial::Update()
{
}

void ComponentMaterial::OnEditor()
{
}

void ComponentMaterial::ReleaseCurrentTexture()
{
    if (textureUID != 0) {
        LOG_CONSOLE("[ComponentMaterial] Releasing texture UID %llu", textureUID);
        Application::GetInstance().resources->ReleaseResource(textureUID);
        textureUID = 0;
    }

    texturePath = "";
}

bool ComponentMaterial::LoadTextureByUID(UID uid)
{
    if (uid == 0) {
        LOG_DEBUG("[ComponentMaterial] Invalid UID");
        return false;
    }

    LOG_CONSOLE("[ComponentMaterial] LoadTextureByUID(%llu) for GameObject: %s",
        uid, owner ? owner->GetName().c_str() : "NULL");

    // Release current texture
    ReleaseCurrentTexture();

    // Request texture resource
    ResourceTexture* texResource = dynamic_cast<ResourceTexture*>(
        Application::GetInstance().resources->RequestResource(uid)
        );

    if (!texResource) {
        LOG_CONSOLE("[ComponentMaterial] ERROR: Failed to load texture with UID: %llu", uid);
        return false;
    }

    if (!texResource->IsLoadedToMemory()) {
        LOG_CONSOLE("[ComponentMaterial] ERROR: Texture not loaded into memory for UID: %llu", uid);
        Application::GetInstance().resources->ReleaseResource(uid);
        return false;
    }

    // Store UID
    textureUID = uid;
    originalTextureUID = uid;

    // Update path for compatibility
    texturePath = texResource->GetAssetFile();
    originalTexturePath = texturePath;

    LOG_CONSOLE("[ComponentMaterial] ✓ Texture loaded successfully!");
    LOG_CONSOLE("  ✓ UID: %llu", uid);
    LOG_CONSOLE("  ✓ Size: %ux%u", texResource->GetWidth(), texResource->GetHeight());
    LOG_CONSOLE("  ✓ GPU ID: %u", texResource->GetGPU_ID());
    LOG_CONSOLE("  ✓ References: %u", texResource->GetReferenceCount());

    return true;
}

bool ComponentMaterial::LoadTexture(const std::string& path)
{
    LOG_CONSOLE("[ComponentMaterial] LoadTexture(\"%s\") for GameObject: %s",
        path.c_str(), owner ? owner->GetName().c_str() : "NULL");

    ModuleResources* resources = Application::GetInstance().resources.get();
    if (!resources) {
        LOG_CONSOLE("[ComponentMaterial] ERROR: ModuleResources not available");
        return false;
    }

    UID uid = resources->Find(path.c_str());

    if (uid == 0) {
        // No existe, intentar importar
        LOG_CONSOLE("[ComponentMaterial] Texture not registered, importing...");
        uid = resources->ImportFile(path.c_str());

        if (uid == 0) {
            LOG_CONSOLE("[ComponentMaterial] ERROR: Failed to import texture");
            return false;
        }

        LOG_CONSOLE("[ComponentMaterial] Texture imported with UID: %llu", uid);
    }

    return LoadTextureByUID(uid);
}

void ComponentMaterial::CreateCheckerboardTexture()
{
    // Release current texture
    ReleaseCurrentTexture();

    texturePath = "[Checkerboard Pattern]";
    originalTexturePath = "";

    LOG_CONSOLE("[ComponentMaterial] Checkerboard texture set");
}

void ComponentMaterial::Use()
{
    if (textureUID == 0) {
        // No texture, unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    // Get texture resource
    const Resource* resource = Application::GetInstance().resources->GetResource(textureUID);

    if (!resource || !resource->IsLoadedToMemory()) {
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    const ResourceTexture* texResource = dynamic_cast<const ResourceTexture*>(resource);
    if (texResource) {
        glBindTexture(GL_TEXTURE_2D, texResource->GetGPU_ID());
    }
    else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void ComponentMaterial::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

int ComponentMaterial::GetTextureWidth() const
{
    if (textureUID == 0) return 0;

    const Resource* resource = Application::GetInstance().resources->GetResource(textureUID);
    if (!resource || !resource->IsLoadedToMemory()) return 0;

    const ResourceTexture* texResource = dynamic_cast<const ResourceTexture*>(resource);
    return texResource ? texResource->GetWidth() : 0;
}

int ComponentMaterial::GetTextureHeight() const
{
    if (textureUID == 0) return 0;

    const Resource* resource = Application::GetInstance().resources->GetResource(textureUID);
    if (!resource || !resource->IsLoadedToMemory()) return 0;

    const ResourceTexture* texResource = dynamic_cast<const ResourceTexture*>(resource);
    return texResource ? texResource->GetHeight() : 0;
}

void ComponentMaterial::RestoreOriginalTexture()
{
    if (originalTextureUID != 0) {
        LoadTextureByUID(originalTextureUID);
        LOG_CONSOLE("[ComponentMaterial] Original texture restored (UID: %llu)", originalTextureUID);
    }
    else {
        LOG_CONSOLE("[ComponentMaterial] No original texture to restore");
        CreateCheckerboardTexture();
    }
}