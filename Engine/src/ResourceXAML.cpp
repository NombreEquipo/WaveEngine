#include "ResourceXAML.h"
#include "XAMLImporter.h"
#include "Log.h"

ResourceXAML::ResourceXAML(UID uid)
    : Resource(uid, Resource::XAML)
{
}

ResourceXAML::~ResourceXAML()
{
    UnloadFromMemory();
}

bool ResourceXAML::LoadInMemory()
{
    if (data)
        return true; // already loaded

    if (libraryFile.empty())
    {
        LOG_CONSOLE("[ResourceXAML] No library path set for UID %llu", uid);
        return false;
    }

    auto blob = XAMLImporter::LoadFromCustomFormat(uid);
    if (!blob.IsValid())
    {
        LOG_CONSOLE("[ResourceXAML] Failed to load binary XAML from: %s", libraryFile.c_str());
        return false;
    }

    // Take ownership of the blob data
    data     = blob.data;
    dataSize = blob.dataSize;
    blob.data     = nullptr; // prevent double-free
    blob.dataSize = 0;

    LOG_DEBUG("[ResourceXAML] Loaded %zu bytes for UID %llu", dataSize, uid);
    return true;
}

void ResourceXAML::UnloadFromMemory()
{
    delete[] data;
    data     = nullptr;
    dataSize = 0;
}
