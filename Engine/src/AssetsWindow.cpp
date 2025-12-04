#include "AssetsWindow.h"
#include <imgui.h>
#include "Application.h"
#include "LibraryManager.h"
#include "MetaFile.h"
#include "ModuleResources.h"
#include "Log.h"  

AssetsWindow::AssetsWindow()
    : EditorWindow("Assets"), selectedAsset(nullptr), iconSize(64.0f),
    showInMemoryOnly(false), showDeleteConfirmation(false)
{
    if (!LibraryManager::IsInitialized()) {
        LibraryManager::Initialize();
    }

    assetsRootPath = LibraryManager::GetAssetsRoot();
    currentPath = assetsRootPath;

    LOG_CONSOLE("Assets root path: %s", assetsRootPath.c_str());
    LOG_CONSOLE("Absolute path: %s", fs::absolute(currentPath).string().c_str());
}

void AssetsWindow::Draw()
{
    if (!isOpen) return;

    static bool firstDraw = true;
    if (firstDraw) {
        RefreshAssets();
        firstDraw = false;
    }

    // Delete confirmation popup
    if (showDeleteConfirmation) {
        ImGui::OpenPopup("Delete Asset?");
        showDeleteConfirmation = false;
    }

    if (ImGui::BeginPopupModal("Delete Asset?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete:");
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "%s", assetToDelete.name.c_str());
        ImGui::Separator();

        if (assetToDelete.isDirectory) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "This will delete the entire folder and all its contents!");
        }
        else {
            ImGui::Text("This will also delete the corresponding Library file(s).");
        }

        ImGui::Separator();

        if (ImGui::Button("Delete", ImVec2(120, 0))) {
            if (DeleteAsset(assetToDelete)) {
                LOG_CONSOLE("Deleted: %s", assetToDelete.name.c_str());
                RefreshAssets();
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (ImGui::Begin(name.c_str(), &isOpen))
    {
        isHovered = ImGui::IsWindowHovered();

        // toolbar
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

        if (ImGui::Button("Refresh"))
        {
            RefreshAssets();
        }

        ImGui::SameLine();
        ImGui::Checkbox("In Memory Only", &showInMemoryOnly);

        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.0f);
        ImGui::SliderFloat("Icon Size", &iconSize, 32.0f, 128.0f, "%.0f");

        ImGui::PopStyleVar();
        ImGui::Separator();

        ImGui::Text("Path: ");
        ImGui::SameLine();

        fs::path relativePath = fs::relative(currentPath, assetsRootPath);
        if (relativePath == ".")
        {
            ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Assets");
        }
        else
        {
            if (ImGui::SmallButton("Assets##BreadcrumbRoot"))
            {
                currentPath = assetsRootPath;
                RefreshAssets();
            }

            std::string pathStr = relativePath.string();
            size_t pos = 0;
            std::string token;
            fs::path accumulatedPath = assetsRootPath;

            while ((pos = pathStr.find("\\")) != std::string::npos || (pos = pathStr.find("/")) != std::string::npos)
            {
                token = pathStr.substr(0, pos);
                ImGui::SameLine();
                ImGui::Text("/");
                ImGui::SameLine();

                accumulatedPath /= token;

                ImGui::PushID(accumulatedPath.string().c_str());
                if (ImGui::SmallButton(token.c_str()))
                {
                    currentPath = accumulatedPath.string();
                    RefreshAssets();
                }
                ImGui::PopID();

                pathStr.erase(0, pos + 1);
            }

            if (!pathStr.empty())
            {
                ImGui::SameLine();
                ImGui::Text("/");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), pathStr.c_str());
            }
        }

        ImGui::Separator();

        // split view
        ImGui::BeginChild("FolderTree", ImVec2(200, 0), true);
        DrawFolderTree(assetsRootPath, "Assets");
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("AssetsList", ImVec2(0, 0), true);
        DrawAssetsList();
        ImGui::EndChild();
    }
    ImGui::End();
}

void AssetsWindow::DrawFolderTree(const fs::path& path, const std::string& label)
{
    if (!fs::exists(path) || !fs::is_directory(path))
        return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (path == currentPath)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool hasSubfolders = false;
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_directory()) {
                hasSubfolders = true;
                break;
            }
        }
    }
    catch (...) {}

    if (!hasSubfolders)
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    ImGui::PushID(path.string().c_str());

    bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), flags);

    if (ImGui::IsItemClicked())
    {
        LOG_DEBUG("CLICKED: %s", path.string().c_str());
        currentPath = path.string();
        RefreshAssets();
    }

    if (nodeOpen)
    {
        if (hasSubfolders)
        {
            try {
                for (const auto& entry : fs::directory_iterator(path))
                {
                    if (entry.is_directory())
                    {
                        std::string folderName = entry.path().filename().string();
                        DrawFolderTree(entry.path(), folderName);
                    }
                }
            }
            catch (...) {}

            LOG_DEBUG("DEBUG: TreePop for %s", label.c_str());
            ImGui::TreePop();
        }
        else
        {
            LOG_DEBUG("DEBUG: Node open but LEAF (No TreePop needed) for %s", label.c_str());
        }
    }

    ImGui::PopID();
}

void AssetsWindow::DrawAssetsList()
{
    if (currentPath != assetsRootPath)
    {
        if (ImGui::Button("<- Back"))
        {
            currentPath = fs::path(currentPath).parent_path().string();
            RefreshAssets();
        }
        ImGui::Separator();
    }

    float windowWidth = ImGui::GetContentRegionAvail().x;
    int columns = (int)(windowWidth / (iconSize + 10.0f));
    if (columns < 1) columns = 1;

    int currentColumn = 0;
    std::string pathPendingToLoad = "";

    for (auto& asset : currentAssets)
    {
        if (showInMemoryOnly && !asset.inMemory)
            continue;

        DrawAssetItem(asset, pathPendingToLoad);

        currentColumn++;
        if (currentColumn < columns)
        {
            ImGui::SameLine();
        }
        else
        {
            currentColumn = 0;
        }
    }

    if (!pathPendingToLoad.empty())
    {
        currentPath = pathPendingToLoad;
        RefreshAssets();
    }
}

void AssetsWindow::DrawAssetItem(const AssetEntry& asset, std::string& pathPendingToLoad)
{
    ImGui::PushID(asset.path.c_str());
    ImGui::BeginGroup();

    const char* icon = GetAssetIcon(asset.extension);

    ImVec4 buttonColor = asset.isDirectory ?
        ImVec4(0.8f, 0.7f, 0.3f, 1.0f) :
        (asset.inMemory ? ImVec4(0.3f, 0.8f, 0.3f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);

    if (ImGui::Button(icon, ImVec2(iconSize, iconSize)))
    {
        if (asset.isDirectory)
        {
            pathPendingToLoad = asset.path;
        }
        else
        {
            selectedAsset = const_cast<AssetEntry*>(&asset);
            LOG_CONSOLE("Selected: %s", asset.name.c_str());
        }
    }

    ImGui::PopStyleColor();

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        if (!asset.isDirectory)
        {
            LOG_CONSOLE("Opening: %s", asset.path.c_str());
        }
        else
        {
            pathPendingToLoad = asset.path;
        }
    }

    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + iconSize);
    ImGui::TextWrapped("%s", asset.name.c_str());
    ImGui::PopTextWrapPos();

    if (asset.inMemory)
    {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Refs: %d", asset.references);
    }

    ImGui::EndGroup();

    // Context Menu with Delete option
    std::string popupID = "AssetContextMenu##" + asset.path;
    if (ImGui::BeginPopupContextItem(popupID.c_str()))
    {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%s", asset.name.c_str());
        ImGui::Separator();

        if (ImGui::MenuItem("Delete"))
        {
            assetToDelete = asset;
            showDeleteConfirmation = true;
        }

        if (!asset.isDirectory && asset.uid != 0)
        {
            ImGui::Separator();
            ImGui::Text("UID: %llu", asset.uid);

            if (asset.inMemory)
            {
                ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Loaded in memory");
            }
        }

        ImGui::EndPopup();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s", asset.name.c_str());
        if (!asset.isDirectory && asset.uid != 0) {
            ImGui::Text("UID: %llu", asset.uid);
        }
        ImGui::EndTooltip();
    }

    ImGui::PopID();
}

bool AssetsWindow::DeleteAsset(const AssetEntry& asset)
{
    LOG_CONSOLE("[AssetsWindow] Deleting asset: %s", asset.path.c_str());

    try {
        if (asset.isDirectory)
        {
            return DeleteDirectory(fs::path(asset.path));
        }
        else
        {
            // 1. Load .meta file to get UID and library paths
            std::string metaPath = asset.path + ".meta";
            unsigned long long uid = 0;
            std::vector<std::string> libraryPaths;

            if (fs::exists(metaPath)) {
                MetaFile meta = MetaFile::Load(metaPath);
                uid = meta.uid;
                libraryPaths = meta.GetAllLibraryPaths();
            }

            // 2. Delete library files
            for (const auto& libPath : libraryPaths) {
                if (!libPath.empty() && fs::exists(libPath)) {
                    LOG_CONSOLE("[AssetsWindow] Deleting library file: %s", libPath.c_str());
                    fs::remove(libPath);
                }
            }

            // 3. Delete .meta file
            if (fs::exists(metaPath)) {
                LOG_CONSOLE("[AssetsWindow] Deleting meta file: %s", metaPath.c_str());
                fs::remove(metaPath);
            }

            // 4. Delete asset file
            if (fs::exists(asset.path)) {
                LOG_CONSOLE("[AssetsWindow] Deleting asset file: %s", asset.path.c_str());
                fs::remove(asset.path);
            }

            // 5. Remove from ModuleResources
            if (uid != 0 && Application::GetInstance().resources) {
                Application::GetInstance().resources->RemoveResource(uid);
            }

            return true;
        }
    }
    catch (const fs::filesystem_error& e) {
        LOG_CONSOLE("[AssetsWindow] ERROR deleting asset: %s", e.what());
        return false;
    }
}

bool AssetsWindow::DeleteDirectory(const fs::path& dirPath)
{
    LOG_CONSOLE("[AssetsWindow] Deleting directory: %s", dirPath.string().c_str());

    try {
        // 1. Collect UIDs and library paths from .meta files
        std::vector<std::pair<unsigned long long, std::vector<std::string>>> filesToDelete;

        for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".meta") {
                MetaFile meta = MetaFile::Load(entry.path().string());
                if (meta.uid != 0) {
                    filesToDelete.push_back({ meta.uid, meta.GetAllLibraryPaths() });
                }
            }
        }

        // 2. Delete library files and remove from resources
        for (const auto& [uid, libraryPaths] : filesToDelete) {
            for (const auto& libPath : libraryPaths) {
                if (!libPath.empty() && fs::exists(libPath)) {
                    LOG_CONSOLE("[AssetsWindow] Deleting library file: %s", libPath.c_str());
                    fs::remove(libPath);
                }
            }

            if (Application::GetInstance().resources) {
                Application::GetInstance().resources->RemoveResource(uid);
            }
        }

        // 3. Delete entire directory
        fs::remove_all(dirPath);

        LOG_CONSOLE("[AssetsWindow] Directory deleted successfully");
        return true;
    }
    catch (const fs::filesystem_error& e) {
        LOG_CONSOLE("[AssetsWindow] ERROR deleting directory: %s", e.what());
        return false;
    }
}

void AssetsWindow::RefreshAssets()
{
    currentAssets.clear();

    if (!fs::exists(currentPath))
    {
        LOG_DEBUG("[AssetsWindow] Path does not exist: %s", currentPath.c_str());
        return;
    }

    ScanDirectory(currentPath, currentAssets);

    LOG_DEBUG("[AssetsWindow] Refreshed: %d assets found", (int)currentAssets.size());
}

void AssetsWindow::ScanDirectory(const fs::path& directory, std::vector<AssetEntry>& outAssets)
{
    if (!fs::exists(directory))
        return;

    for (const auto& entry : fs::directory_iterator(directory))
    {
        const auto& path = entry.path();
        std::string filename = path.filename().string();
        std::string extension = path.extension().string();

        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (extension == ".meta")
        {
            continue;
        }

        bool isDirectory = entry.is_directory();

        if (!isDirectory && !IsAssetFile(extension))
        {
            continue;
        }

        AssetEntry asset;
        asset.name = filename;
        asset.path = path.string();
        asset.isDirectory = isDirectory;
        asset.extension = extension;
        asset.inMemory = false;
        asset.references = 0;
        asset.uid = 0;

        std::string metaPath = asset.path + ".meta";
        if (fs::exists(metaPath))
        {
            MetaFile meta = MetaFile::Load(metaPath);
            asset.uid = meta.uid;
        }

        outAssets.push_back(asset);
    }

    std::sort(outAssets.begin(), outAssets.end(), [](const AssetEntry& a, const AssetEntry& b)
        {
            if (a.isDirectory != b.isDirectory)
                return a.isDirectory > b.isDirectory;
            return a.name < b.name;
        });
}

const char* AssetsWindow::GetAssetIcon(const std::string& extension) const
{
    if (extension == ".fbx" || extension == ".obj")
        return "MODEL";
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".dds")
        return "IMG";
    if (extension == ".mesh")
        return "MESH";
    if (extension == ".texture")
        return "TEX";
    if (extension == ".wav" || extension == ".ogg" || extension == ".mp3")
        return "AUDIO";
    if (extension.empty())
        return "FOLDER";

    return "FILE";
}

bool AssetsWindow::IsAssetFile(const std::string& extension) const
{
    return extension == ".fbx" ||
        extension == ".obj" ||
        extension == ".png" ||
        extension == ".jpg" ||
        extension == ".jpeg" ||
        extension == ".dds" ||
        extension == ".tga" ||
        extension == ".mesh" ||
        extension == ".texture" ||
        extension == ".wav" ||
        extension == ".ogg";
}