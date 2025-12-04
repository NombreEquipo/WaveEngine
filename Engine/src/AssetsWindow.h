#pragma once

#include "EditorWindow.h"
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

struct AssetEntry
{
    std::string name;
    std::string path;
    bool isDirectory;
    std::string extension;
    unsigned long long uid;
    bool inMemory;
    int references;
};

class AssetsWindow : public EditorWindow
{
public:
    AssetsWindow();
    ~AssetsWindow() override = default;

    void Draw() override;

private:
    void DrawFolderTree(const fs::path& path, const std::string& label);
    void DrawAssetsList();
    void DrawAssetItem(const AssetEntry& asset, std::string& pathPendingToLoad);
    void RefreshAssets();
    void ScanDirectory(const fs::path& directory, std::vector<AssetEntry>& outAssets);

    bool DeleteAsset(const AssetEntry& asset);
    bool DeleteDirectory(const fs::path& dirPath);

    const char* GetAssetIcon(const std::string& extension) const;
    bool IsAssetFile(const std::string& extension) const;

private:
    std::string assetsRootPath;
    std::string currentPath;
    std::vector<AssetEntry> currentAssets;
    AssetEntry* selectedAsset;
    float iconSize;
    bool showInMemoryOnly;

    bool showDeleteConfirmation;
    AssetEntry assetToDelete;
};