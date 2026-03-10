#include "CubemapImporter.h"
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

    MaterialType type = (MaterialType)j.value("Type", (int)MaterialType::STANDARD);

    Material* mat = nullptr;
    switch (type) {
    case MaterialType::STANDARD: mat = new MaterialStandard(type); break;
    }

    if (mat) {
        mat->SetOpacity(j.value("Opacity", 1.0f));
        mat->LoadFromJson(j);

        bool succes = SaveToCustomFormat(*mat, uid);

        delete mat;
        return succes;
    }

    return false;
}

bool CubemapImporter::SaveToCustomFormat(const Cubemap& mat, const UID& uid)
{

    return true;
}

Cubemap* CubemapImporter::LoadFromCustomFormat(const UID& uid)
{

    return ;
}

UID CubemapImporter::CreateNewCubemap(const std::string& directory, const std::string& name) {

    
    return success ? newUID : 0;
}

Cubemap* CubemapImporter::CloneCubemap(const Cubemap* source) {
    if (!source) return nullptr;

    nlohmann::json tmp;
    tmp["Type"] = (int)source->GetType();
    tmp["Opacity"] = source->GetOpacity();
    source->SaveToJson(tmp);

    Material* copy = nullptr;
    switch (source->GetType()) {
    case MaterialType::STANDARD: copy = new MaterialStandard(MaterialType::STANDARD); break;
    }

    if (copy) {
        copy->LoadFromJson(tmp);
    }

    return copy;
}