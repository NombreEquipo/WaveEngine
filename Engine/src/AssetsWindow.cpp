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

    static bool firstDraw = true;
    if (firstDraw) {
        RefreshAssets();
        firstDraw = false;
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
            // ID ÚNICO para el botón raíz
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

                // ID ÚNICO para cada botón de carpeta usando PushID
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

    // 1. PUSH ID
    ImGui::PushID(path.string().c_str());

    // 2. DIBUJAR NODO
    bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), flags);

    // 3. CLICK LOGIC
    if (ImGui::IsItemClicked())
    {
        LOG_DEBUG("CLICKED: %s", path.string().c_str());
        currentPath = path.string();
        RefreshAssets();
    }

    // 4. RECURSIVIDAD
    if (nodeOpen)
    {
        if (hasSubfolders)
        {
            // LOG_CONSOLE("DEBUG: Entering recursion for %s", label.c_str());
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

    // 5. POP ID
    ImGui::PopID();
    //LOG_DEBUG("DEBUG: PopID done for %s", label.c_str());
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

    std::string pathPendingToLoad = "";

    for (auto& asset : currentAssets)
    {
        if (showInMemoryOnly && !asset.inMemory)
            continue;

        // Pasamos la variable por referencia
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
    // ID Scope Único para todo el item basado en su ruta
    ImGui::PushID(asset.path.c_str());
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
            pathPendingToLoad = asset.path;
        }
        else
        {
            selectedAsset = const_cast<AssetEntry*>(&asset);
            LOG_CONSOLE("Selected: %s", asset.name.c_str());
        }
    }

    ImGui::PopStyleColor();

    // Doble clic
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

    // Nombre
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + iconSize);

    ImGui::TextWrapped("%s", asset.name.c_str());

    ImGui::PopTextWrapPos();

    // Info memoria
    if (asset.inMemory)
    {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Refs: %d", asset.references);
    }

    ImGui::EndGroup();

    // Context Menu (abreviado para que veas donde va)
    std::string popupID = "AssetContextMenu##" + asset.path;
    if (ImGui::BeginPopupContextItem(popupID.c_str()))
    {
        ImGui::EndPopup();
    }

    // Tooltip
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s", asset.name.c_str());
        ImGui::EndTooltip();
    }

    ImGui::PopID();
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
    // LOG_CONSOLE("=== Scanning directory: %s ===", directory.string().c_str());

    if (!fs::exists(directory))
        return;

    for (const auto& entry : fs::directory_iterator(directory))
    {
        const auto& path = entry.path();
        std::string filename = path.filename().string();
        std::string extension = path.extension().string();

        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char c) { return std::tolower(c); });

        // Ignorar archivos .meta
        if (extension == ".meta")
        {
            // LOG_CONSOLE("Found entry: %s (isDir: 0) -> Skipped (meta file)", filename.c_str());
            continue;
        }

        bool isDirectory = entry.is_directory();

        if (!isDirectory)
        {
            if (!IsAssetFile(extension))
            {
                LOG_CONSOLE("Found entry: %s -> Skipped (unknown extension: '%s')", filename.c_str(), extension.c_str());
                continue;
            }
        }

        AssetEntry asset;
        asset.name = filename;
        asset.path = path.string();
        asset.isDirectory = isDirectory;
        asset.extension = extension;
        asset.inMemory = false;
        asset.references = 0;
        asset.uid = 0;

        // Intentar obtener UID del archivo .meta si existe
        std::string metaPath = asset.path + ".meta";
        if (fs::exists(metaPath))
        {
            // Aquí deberías leer el UID real del meta. 
            // Por ahora simulamos o leemos si tienes la función implementada.
            //asset.uid = LoadUIDFromMeta(metaPath); 
        }

        // Check if loaded in memory (en  ModuleResources)
        // if (!isDirectory && App->resources->IsResourceLoaded(asset.uid)) { ... }

        outAssets.push_back(asset);

        // LOG_CONSOLE("Found entry: %s (isDir: %d) -> Added", filename.c_str(), isDirectory);
    }

    // Ordenar: Carpetas primero, luego alfabéticamente
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