#include "MetaFile.h"
#include "LibraryManager.h"
#include <random>
#include <iomanip>
#include <iostream>
#include <windows.h>

// MetaFile Implementation

std::string MetaFile::GenerateGUID() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    uint64_t part1 = dis(gen);
    uint64_t part2 = dis(gen);

    std::stringstream ss;
    ss << std::hex << std::setfill('0')
        << std::setw(16) << part1
        << std::setw(16) << part2;

    return ss.str();
}

AssetType MetaFile::GetAssetType(const std::string& extension) {
    std::string ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".fbx") return AssetType::MODEL_FBX;
    if (ext == ".png") return AssetType::TEXTURE_PNG;
    if (ext == ".jpg" || ext == ".jpeg") return AssetType::TEXTURE_JPG;
    if (ext == ".dds") return AssetType::TEXTURE_DDS;

    return AssetType::UNKNOWN;
}

bool MetaFile::Save(const std::string& metaFilePath) const {
    std::ofstream file(metaFilePath);
    if (!file.is_open()) {
        std::cerr << "[MetaFile] ERROR: Cannot create .meta file: " << metaFilePath << std::endl;
        return false;
    }

    // Formato simple key-value
    file << "guid: " << guid << "\n";
    file << "type: " << static_cast<int>(type) << "\n";
    file << "originalPath: " << originalPath << "\n";
    file << "libraryPath: " << libraryPath << "\n";
    file << "lastModified: " << lastModified << "\n";
    file << "importScale: " << importSettings.importScale << "\n";
    file << "generateNormals: " << importSettings.generateNormals << "\n";
    file << "flipUVs: " << importSettings.flipUVs << "\n";
    file << "optimizeMeshes: " << importSettings.optimizeMeshes << "\n";

    file.close();
    return true;
}

MetaFile MetaFile::Load(const std::string& metaFilePath) {
    MetaFile meta;

    std::ifstream file(metaFilePath);
    if (!file.is_open()) {
        return meta; // Retorna meta vacío si falla
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 2); // +2 para saltar ": "

        if (key == "guid") meta.guid = value;
        else if (key == "type") meta.type = static_cast<AssetType>(std::stoi(value));
        else if (key == "originalPath") meta.originalPath = value;
        else if (key == "libraryPath") meta.libraryPath = value;
        else if (key == "lastModified") meta.lastModified = std::stoll(value);
        else if (key == "importScale") meta.importSettings.importScale = std::stof(value);
        else if (key == "generateNormals") meta.importSettings.generateNormals = (value == "1");
        else if (key == "flipUVs") meta.importSettings.flipUVs = (value == "1");
        else if (key == "optimizeMeshes") meta.importSettings.optimizeMeshes = (value == "1");
    }

    file.close();
    return meta;
}

bool MetaFile::NeedsReimport(const std::string& assetPath) const {
    if (!std::filesystem::exists(assetPath)) {
        return false; // El asset no existe
    }

    long long currentTimestamp = std::filesystem::last_write_time(assetPath).time_since_epoch().count();

    // Si el timestamp cambió, necesita reimportación
    return currentTimestamp != lastModified;
}

// MetaFileManager Implementation

void MetaFileManager::Initialize() {
    std::cout << "[MetaFileManager] Initializing metadata system..." << std::endl;

    // Escanear Assets/ y crear .meta para archivos nuevos
    ScanAssets();

    std::cout << "[MetaFileManager] Metadata system initialized" << std::endl;
}

void MetaFileManager::ScanAssets() {
    std::string assetsPath = LibraryManager::GetAssetsRoot();

    if (!std::filesystem::exists(assetsPath)) {
        std::cerr << "[MetaFileManager] ERROR: Assets folder not found" << std::endl;
        return;
    }

    std::cout << "[MetaFileManager] Scanning Assets folder..." << std::endl;

    int metasCreated = 0;
    int metasUpdated = 0;

    // Iterar recursivamente por Assets/
    for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath)) {
        if (!entry.is_regular_file()) continue;

        std::string assetPath = entry.path().string();
        std::string extension = entry.path().extension().string();

        // Ignorar archivos .meta
        if (extension == ".meta") continue;

        AssetType type = MetaFile::GetAssetType(extension);
        if (type == AssetType::UNKNOWN) continue; // Tipo no soportado

        std::string metaPath = GetMetaPath(assetPath);

        // Verificar si .meta existe
        if (!std::filesystem::exists(metaPath)) {
            // Crear nuevo .meta
            MetaFile meta;
            meta.guid = MetaFile::GenerateGUID();
            meta.type = type;
            meta.originalPath = assetPath;
            meta.lastModified = GetFileTimestamp(assetPath);
            meta.libraryPath = ""; // Se asignará durante importación

            if (meta.Save(metaPath)) {
                std::cout << "  [+] Created .meta: " << entry.path().filename().string() << ".meta" << std::endl;
                metasCreated++;
            }
        }
        else {
            // Verificar si necesita actualización
            MetaFile meta = MetaFile::Load(metaPath);
            long long currentTimestamp = GetFileTimestamp(assetPath);

            if (meta.lastModified != currentTimestamp) {
                std::cout << "  [*] Asset modified: " << entry.path().filename().string() << std::endl;
                metasUpdated++;
            }
        }
    }

    std::cout << "[MetaFileManager] Scan complete: " << metasCreated << " created, "
        << metasUpdated << " need reimport" << std::endl;
}

MetaFile MetaFileManager::GetOrCreateMeta(const std::string& assetPath) {
    std::string metaPath = GetMetaPath(assetPath);

    // Intentar cargar .meta existente
    if (std::filesystem::exists(metaPath)) {
        return MetaFile::Load(metaPath);
    }

    // Crear nuevo .meta
    MetaFile meta;
    meta.guid = MetaFile::GenerateGUID();
    meta.type = MetaFile::GetAssetType(std::filesystem::path(assetPath).extension().string());
    meta.originalPath = assetPath;
    meta.lastModified = GetFileTimestamp(assetPath);

    meta.Save(metaPath);

    return meta;
}

bool MetaFileManager::NeedsReimport(const std::string& assetPath) {
    std::string metaPath = GetMetaPath(assetPath);

    if (!std::filesystem::exists(metaPath)) {
        return true; // No hay .meta, necesita importación
    }

    MetaFile meta = MetaFile::Load(metaPath);
    return meta.NeedsReimport(assetPath);
}

void MetaFileManager::RegenerateLibrary() {
    std::cout << "\n[MetaFileManager] ========================================" << std::endl;
    std::cout << "[MetaFileManager] REGENERATING LIBRARY FROM ASSETS" << std::endl;
    std::cout << "[MetaFileManager] ========================================\n" << std::endl;

    // TODO: Implementar reimportación completa
    // Por ahora, solo escaneamos
    ScanAssets();

    std::cout << "\n[MetaFileManager] Library regeneration complete" << std::endl;
}

// Private Helpers

std::string MetaFileManager::GetMetaPath(const std::string& assetPath) {
    return assetPath + ".meta";
}

long long MetaFileManager::GetFileTimestamp(const std::string& filePath) {
    if (!std::filesystem::exists(filePath)) {
        return 0;
    }

    return std::filesystem::last_write_time(filePath).time_since_epoch().count();
}