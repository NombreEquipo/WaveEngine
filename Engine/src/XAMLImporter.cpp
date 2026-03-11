#include "XAMLImporter.h"
#include "LibraryManager.h"
#include "Log.h"

#include "NoesisPCH.h"
#include "NsCore/Noesis.h"
#include "NsGui/IntegrationAPI.h"

#include <fstream>
#include <filesystem>
#include "ModuleResources.h"

#define XAML_COMPILE_STRATEGY_RAWCOPY   0  
#define XAML_COMPILE_STRATEGY_NOESIS    1  

#define XAML_COMPILE_STRATEGY  XAML_COMPILE_STRATEGY_RAWCOPY

XAMLData XAMLImporter::ImportFromFile(const std::string& xamlPath)
{
    XAMLData result;

    if (!std::filesystem::exists(xamlPath))
    {
        LOG_CONSOLE("[XAMLImporter] File not found: %s", xamlPath.c_str());
        return result;
    }

#if XAML_COMPILE_STRATEGY == XAML_COMPILE_STRATEGY_RAWCOPY

    std::ifstream file(xamlPath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        LOG_CONSOLE("[XAMLImporter] Cannot open: %s", xamlPath.c_str());
        return result;
    }

    const size_t fileSize = static_cast<size_t>(file.tellg());
    if (fileSize == 0 || fileSize > 64 * 1024 * 1024)
    {
        LOG_CONSOLE("[XAMLImporter] Unexpected file size (%zu) for: %s", fileSize, xamlPath.c_str());
        return result;
    }

    file.seekg(0);
    result.data = new unsigned char[fileSize];
    file.read(reinterpret_cast<char*>(result.data), static_cast<std::streamsize>(fileSize));

    if (!file.good())
    {
        LOG_CONSOLE("[XAMLImporter] Read error: %s", xamlPath.c_str());
        delete[] result.data;
        result.data     = nullptr;
        result.dataSize = 0;
        return result;
    }

    result.dataSize = fileSize;
    LOG_DEBUG("[XAMLImporter] Imported (raw-copy) %zu bytes from: %s", fileSize, xamlPath.c_str());

#elif XAML_COMPILE_STRATEGY == XAML_COMPILE_STRATEGY_NOESIS

    Noesis::Ptr<Noesis::FrameworkElement> xaml =
        Noesis::GUI::LoadXaml<Noesis::FrameworkElement>(
            std::filesystem::path(xamlPath).filename().string().c_str());

    if (!xaml)
    {
        LOG_CONSOLE("[XAMLImporter] Noesis failed to load XAML: %s", xamlPath.c_str());
        return result;
    }

    // SaveBinary writes to a Noesis stream — wrap a memory buffer
    Noesis::MemoryStream stream;
    if (!Noesis::GUI::SaveBinary(xaml, stream))
    {
        LOG_CONSOLE("[XAMLImporter] Noesis SaveBinary failed for: %s", xamlPath.c_str());
        return result;
    }

    result.dataSize = stream.GetSize();
    result.data     = new unsigned char[result.dataSize];
    memcpy(result.data, stream.GetPtr(), result.dataSize);
    LOG_DEBUG("[XAMLImporter] Compiled to %zu bytes: %s", result.dataSize, xamlPath.c_str());
#endif

    return result;
}

bool XAMLImporter::SaveToCustomFormat(const XAMLData& xamlData, UID uid)
{
    if (!xamlData.IsValid())
    {
        LOG_CONSOLE("[XAMLImporter] SaveToCustomFormat: invalid data for UID %llu", uid);
        return false;
    }

    const std::string fullPath = LibraryManager::GetLibraryPathFromUID(uid);

    std::ofstream file(fullPath, std::ios::binary);
    if (!file.is_open())
    {
        LOG_CONSOLE("[XAMLImporter] Cannot open for writing: %s", fullPath.c_str());
        return false;
    }

    XAMLHeader header;
    header.dataSize = static_cast<uint64_t>(xamlData.dataSize);

    file.write(reinterpret_cast<const char*>(&header), sizeof(XAMLHeader));
    file.write(reinterpret_cast<const char*>(xamlData.data),
               static_cast<std::streamsize>(xamlData.dataSize));

    if (!file.good())
    {
        LOG_CONSOLE("[XAMLImporter] Write error: %s", fullPath.c_str());
        return false;
    }

    LOG_DEBUG("[XAMLImporter] Saved %llu bytes to: %s", header.dataSize, fullPath.c_str());
    return true;
}

XAMLData XAMLImporter::LoadFromCustomFormat(UID uid)
{
    XAMLData result;

    const std::string fullPath = LibraryManager::GetLibraryPathFromUID(uid);

    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open())
    {
        LOG_CONSOLE("[XAMLImporter] Cannot open for reading: %s", fullPath.c_str());
        return result;
    }

    XAMLHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(XAMLHeader));

    if (!file.good())
    {
        LOG_CONSOLE("[XAMLImporter] Header read error: %s", fullPath.c_str());
        return result;
    }

    // Validate magic
    if (header.magic[0] != 'X' || header.magic[1] != 'A' ||
        header.magic[2] != 'M' || header.magic[3] != 'B')
    {
        LOG_CONSOLE("[XAMLImporter] Invalid magic in: %s", fullPath.c_str());
        return result;
    }

    if (header.dataSize == 0 || header.dataSize > 64 * 1024 * 1024)
    {
        LOG_CONSOLE("[XAMLImporter] Suspicious data size (%llu) in: %s",
                    header.dataSize, fullPath.c_str());
        return result;
    }

    result.data     = new unsigned char[static_cast<size_t>(header.dataSize)];
    result.dataSize = static_cast<size_t>(header.dataSize);

    file.read(reinterpret_cast<char*>(result.data),
              static_cast<std::streamsize>(header.dataSize));

    if (!file.good())
    {
        LOG_CONSOLE("[XAMLImporter] Data read error: %s", fullPath.c_str());
        delete[] result.data;
        result.data     = nullptr;
        result.dataSize = 0;
        return result;
    }

    LOG_DEBUG("[XAMLImporter] Loaded %zu bytes from: %s", result.dataSize, fullPath.c_str());
    return result;
}
