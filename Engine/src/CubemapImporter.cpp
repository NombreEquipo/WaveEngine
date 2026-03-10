#include "CubemapImporter.h"
#include "Cubemap.h"
#include "LibraryManager.h"
#include "Application.h"
#include "ModuleResources.h"
#include "Globals.h"

#include <fstream>
#include "nlohmann/json.hpp"

CubemapImporter::CubemapImporter() {}
CubemapImporter::~CubemapImporter() {}

bool CubemapImporter::ImportCubemap(const std::string& path, const UID& uid)
{
    std::ifstream file(path);
    if (!file.is_open()) return false;

    nlohmann::json j;
    try {
        file >> j;
    }
    catch (nlohmann::json::parse_error& e) {
        return false;
    }

    Cubemap* cubemap = new Cubemap();

    if (cubemap) {
        cubemap->LoadFromJson(j);

        bool succes = SaveToCustomFormat(*cubemap, uid);

        delete cubemap;

        return succes;
    }

    return false;
}

bool CubemapImporter::SaveToCustomFormat(const Cubemap& cubemap, const UID& uid)
{
    std::string fullPath = LibraryManager::GetLibraryPathFromUID(uid);
    std::ofstream file(fullPath, std::ios::binary);

    if (!file.is_open()) return false;

    cubemap.SaveCustomData(file);

    file.close();

    return true;
}

Cubemap* CubemapImporter::LoadFromCustomFormat(const UID& uid)
{
    std::string fullPath = LibraryManager::GetLibraryPathFromUID(uid);
    std::ifstream file(fullPath, std::ios::binary);

    if (!file.is_open()) return nullptr;

    Cubemap* cubemap = new Cubemap();

    if (cubemap) {
        cubemap->LoadCustomData(file);
    }

    file.close();

    return cubemap;
}

UID CubemapImporter::CreateNewCubemap(const std::string& directory, const std::string& name)
{
    Cubemap* cubemap = new Cubemap();
    UID newUID = GenerateUID();
    std::string fileName = "";
    if (name.size() != 0)
        fileName = directory + "/" + name + ".cubemap";
    else
        fileName = directory + "/NewSkybox_" + std::to_string(newUID) + ".cubemap";

    if (std::filesystem::exists(fileName))
        LOG_CONSOLE("Unable to create cubemap, %s already exists", fileName.c_str());

    nlohmann::json j;
    cubemap->SaveToJson(j);

    bool success = false;
    std::ofstream file(fileName);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
        success = true;
    }

    Application::GetInstance().resources.get()->ImportFile(fileName.c_str(), true);

    delete cubemap;

    return success ? newUID : 0;
}

Cubemap* CubemapImporter::CloneCubemap(const Cubemap* source) {
    if (!source) return nullptr;

    nlohmann::json tmp;
    source->SaveToJson(tmp);

    Cubemap* copy = nullptr;

    if (copy) {
        copy->LoadFromJson(tmp);
    }

    return copy;
}