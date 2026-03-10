#include "ResourceCubemap.h"
#include "ModuleResources.h"
#include "CubemapImporter.h"
#include "Log.h"
#include <fstream>

ResourceCubemap::ResourceCubemap(UID uid)
    : Resource(uid, Resource::CUBEMAP) {
}

ResourceCubemap::~ResourceCubemap() {
    UnloadFromMemory();
}

bool ResourceCubemap::LoadInMemory()
{
    cubemap = CubemapImporter::LoadFromCustomFormat(uid);
    return true;
}

void ResourceCubemap::UnloadFromMemory()
{
    if (cubemap != nullptr) {
        delete cubemap;
        cubemap = nullptr;
    }
}