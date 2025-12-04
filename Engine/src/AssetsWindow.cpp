#include "AssetsWindow.h"
#include <imgui.h>
#include "Application.h"
#include "LibraryManager.h"
#include "MetaFile.h"
#include "ModuleResources.h"
#include "Log.h"

AssetsWindow::AssetsWindow()
    : EditorWindow("Assets"), selectedAsset(nullptr), iconSize(64.0f), showInMemoryOnly(false)
{
    // Asegurar que LibraryManager esté inicializado
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

    // Refresh en el primer frame de dibujo si es necesario
    static bool firstDraw = true;
    if (firstDraw) {
        RefreshAssets();
        firstDraw = false;
    }
    if (ImGui::Begin(name.c_str(), &isOpen))
    {
        isHovered = ImGui::IsWindowHovered();

        // Toolbar
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

        // Show current path with breadcrumb navigation
        ImGui::Text("Path: ");
        ImGui::SameLine();

        fs::path relativePath = fs::relative(currentPath, assetsRootPath);
        if (relativePath == ".")
        {
            ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Assets");
        }
        else
        {
            if (ImGui::SmallButton("Assets"))
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
                std::string pathCopy = accumulatedPath.string();

                if (ImGui::SmallButton(token.c_str()))
                {
                    currentPath = pathCopy;
                    RefreshAssets();
                }

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

        // Split view: Folder tree on left, assets grid on right
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
    {
        LOG_CONSOLE("Path doesn't exist or is not directory: %s", path.string().c_str());
        return;
    }

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (path == currentPath)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool hasSubfolders = false;
    int subfolderCount = 0;
    for (const auto& entry : fs::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            hasSubfolders = true;
            subfolderCount++;
        }
    }

    if (!hasSubfolders)
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    ImGui::PushID(path.string().c_str());
    bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), flags);
    ImGui::PopID();

    if (ImGui::IsItemClicked())
    {
        currentPath = path.string();
        LOG_CONSOLE("Clicked folder, new currentPath: %s", currentPath.c_str());
        RefreshAssets();
    }

    if (nodeOpen && hasSubfolders)
    {
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (entry.is_directory())
            {
                std::string folderName = entry.path().filename().string();
                DrawFolderTree(entry.path(), folderName);
            }
        }
        ImGui::TreePop();
    }
}

void AssetsWindow::DrawAssetsList()
{
    // Navigation buttons
    if (currentPath != assetsRootPath)
    {
        if (ImGui::Button("<- Back"))
        {
            currentPath = fs::path(currentPath).parent_path().string();
            RefreshAssets();
        }
        ImGui::Separator();
    }

    // Calculate grid layout
    float windowWidth = ImGui::GetContentRegionAvail().x;
    int columns = (int)(windowWidth / (iconSize + 10.0f));
    if (columns < 1) columns = 1;

    // Display assets in grid
    int currentColumn = 0;

    for (auto& asset : currentAssets)
    {
        if (showInMemoryOnly && !asset.inMemory)
            continue;

        DrawAssetItem(asset);

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
}

void AssetsWindow::DrawAssetItem(const AssetEntry& asset)
{
    ImGui::BeginGroup();

    // Icon button
    const char* icon = GetAssetIcon(asset.extension);

    ImVec4 buttonColor = asset.isDirectory ?
        ImVec4(0.8f, 0.7f, 0.3f, 1.0f) :
        (asset.inMemory ? ImVec4(0.3f, 0.8f, 0.3f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);

    if (ImGui::Button(icon, ImVec2(iconSize, iconSize)))
    {
        if (asset.isDirectory)
        {
            currentPath = asset.path;
            RefreshAssets();
        }
        else
        {
            selectedAsset = const_cast<AssetEntry*>(&asset);
            LOG_CONSOLE("Selected: %s", asset.name.c_str());
        }
    }

    ImGui::PopStyleColor();

    // Double click to open/import
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        if (!asset.isDirectory)
        {
            LOG_CONSOLE("Opening: %s", asset.path.c_str());
        }
    }

    // Show name below icon
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + iconSize);
    ImGui::TextWrapped("%s", asset.name.c_str());
    ImGui::PopTextWrapPos();

    // Show memory info
    if (asset.inMemory)
    {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Refs: %d", asset.references);
    }

    ImGui::EndGroup();

    // Usar la ruta completa del asset como ID único
    std::string popupID = "AssetContextMenu##" + asset.path;
    if (ImGui::BeginPopupContextItem(popupID.c_str()))
    {
        if (!asset.isDirectory)
        {
            if (ImGui::MenuItem("Reimport"))
            {
                LOG_CONSOLE("Reimporting: %s", asset.path.c_str());
                // Trigger reimport
            }

            if (ImGui::MenuItem("Show in Explorer"))
            {
                std::string command = "explorer /select,\"" + asset.path + "\"";
                system(command.c_str());
            }

            ImGui::Separator();

            ImGui::Text("UID: %llu", asset.uid);
            ImGui::Text("In Memory: %s", asset.inMemory ? "Yes" : "No");
            if (asset.inMemory)
            {
                ImGui::Text("References: %d", asset.references);
            }
        }
        else
        {
            if (ImGui::MenuItem("Open in Explorer"))
            {
                std::string command = "explorer \"" + asset.path + "\"";
                system(command.c_str());
            }
        }

        ImGui::EndPopup();
    }

    // Tooltip with full info
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s", asset.name.c_str());
        if (!asset.isDirectory)
        {
            ImGui::Separator();
            ImGui::Text("Type: %s", asset.extension.c_str());
            ImGui::Text("UID: %llu", asset.uid);
            ImGui::Text("In Memory: %s", asset.inMemory ? "Yes" : "No");
            if (asset.inMemory)
            {
                ImGui::Text("References: %d", asset.references);
            }
        }
        ImGui::EndTooltip();
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
    LOG_CONSOLE("=== Scanning directory: %s ===", directory.string().c_str());

    try
    {
        int totalEntries = 0;
        int skippedMeta = 0;
        int skippedExtension = 0;
        int addedAssets = 0;

        for (const auto& entry : fs::directory_iterator(directory))
        {
            totalEntries++;
            AssetEntry asset;
            asset.path = entry.path().string();
            asset.name = entry.path().filename().string();
            asset.isDirectory = entry.is_directory();

            LOG_CONSOLE("Found entry: %s (isDir: %d)", asset.name.c_str(), asset.isDirectory);

            asset.uid = 0;
            asset.inMemory = false;
            asset.references = 0;

            // Skip .meta files
            if (asset.name.find(".meta") != std::string::npos)
            {
                LOG_CONSOLE("  -> Skipped (meta file)");
                skippedMeta++;
                continue;
            }

            if (!asset.isDirectory)
            {
                asset.extension = entry.path().extension().string();
                LOG_CONSOLE("  -> Extension: '%s'", asset.extension.c_str());

                // Skip unknown file types
                if (!IsAssetFile(asset.extension))
                {
                    LOG_CONSOLE("  -> Skipped (unknown extension)");
                    skippedExtension++;
                    continue;
                }

                // Try to get UID from meta file
                asset.uid = MetaFileManager::GetUIDFromAsset(asset.path);

                // Check if resource is in memory
                if (asset.uid != 0)
                {
                    const Resource* resource = Application::GetInstance().resources->RequestResource(asset.uid);
                    if (resource)
                    {
                        asset.inMemory = resource->IsLoadedToMemory();
                        asset.references = resource->GetReferenceCount();
                        Application::GetInstance().resources->ReleaseResource(asset.uid);
                    }
                }
            }

            LOG_CONSOLE("  -> ADDED to list");
            outAssets.push_back(asset);
            addedAssets++;
        }

        LOG_CONSOLE("=== SUMMARY ===");
        LOG_CONSOLE("Total entries found: %d", totalEntries);
        LOG_CONSOLE("Skipped meta files: %d", skippedMeta);
        LOG_CONSOLE("Skipped unknown extensions: %d", skippedExtension);
        LOG_CONSOLE("Assets added: %d", addedAssets);
        LOG_CONSOLE("Final outAssets.size(): %d", (int)outAssets.size());

        // Sort: directories first, then alphabetically
        std::sort(outAssets.begin(), outAssets.end(), [](const AssetEntry& a, const AssetEntry& b)
            {
                if (a.isDirectory != b.isDirectory)
                    return a.isDirectory > b.isDirectory;
                return a.name < b.name;
            });
    }
    catch (const fs::filesystem_error& e)
    {
        LOG_DEBUG("[AssetsWindow] Error scanning directory: %s", e.what());
    }
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