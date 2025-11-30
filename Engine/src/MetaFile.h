#pragma once

#include <string>
#include <chrono>
#include <fstream>
#include <sstream>
#include <filesystem>

// Tipos de assets soportados
enum class AssetType {
    UNKNOWN,
    MODEL_FBX,
    TEXTURE_PNG,
    TEXTURE_JPG,
    TEXTURE_DDS,
    TEXTURE_TGA  
};

// Metadata de un asset
struct MetaFile {
    std::string guid;                    // Identificador único
    AssetType type = AssetType::UNKNOWN; // Tipo de asset
    std::string originalPath;            // Ruta en Assets/
    std::string libraryPath;             // Ruta en Library/
    long long lastModified = 0;          // Timestamp última modificación

    // Configuración de importación (específica por tipo)
    struct ImportSettings {
        float importScale = 1.0f;
        bool generateNormals = true;
        bool flipUVs = true;
        bool optimizeMeshes = true;
    } importSettings;

    // Generar GUID único
    static std::string GenerateGUID();

    // Determinar tipo de asset por extensión
    static AssetType GetAssetType(const std::string& extension);

    // Guardar metadata en archivo .meta
    bool Save(const std::string& metaFilePath) const;

    // Cargar metadata desde archivo .meta
    static MetaFile Load(const std::string& metaFilePath);

    // Verificar si el asset ha sido modificado
    bool NeedsReimport(const std::string& assetPath) const;

    // Convertir ruta absoluta a relativa desde project root
    static std::string MakeRelativeToProject(const std::string& absolutePath);

    // Convertir ruta relativa a absoluta
    static std::string MakeAbsoluteFromProject(const std::string& relativePath);
};

// Manager de archivos .meta
class MetaFileManager {
public:
    // Inicializar sistema de metadata
    static void Initialize();

    // Escanear carpeta Assets/ y crear/actualizar .meta
    static void ScanAssets();

    // Obtener o crear .meta para un asset
    static MetaFile GetOrCreateMeta(const std::string& assetPath);

    // Verificar si un asset necesita reimportarse
    static bool NeedsReimport(const std::string& assetPath);

    // Regenerar Library/ completa desde Assets/
    static void RegenerateLibrary();
    static long long GetFileTimestamp(const std::string& filePath);
private:
    static std::string GetMetaPath(const std::string& assetPath);
};