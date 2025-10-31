#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <IL/il.h>
#include <windows.h>
#include <psapi.h>
#include <gl/GL.h>
#include <SDL3/SDL_timer.h>

#include "ModuleEditor.h"
#include "Application.h"
#include "Log.h"
#include "GameObject.h"
#include "SelectionManager.h"
#include "ModuleScene.h"
#include "Primitives.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Transform.h"


ModuleEditor::ModuleEditor() : Module()
{
    name = "ModuleEditor";
    fpsHistory.reserve(maxFPSHistory);
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Start()
{
    LOG_DEBUG("Initializing Editor");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //io.IniFilename = NULL;  // for testing
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL3_InitForOpenGL(Application::GetInstance().window->GetWindow(), Application::GetInstance().renderContext->GetContext());
    ImGui_ImplOpenGL3_Init();

    ImGui::StyleColorsDark();

    LOG_CONSOLE("Editor initialized");

    return true;
}

bool ModuleEditor::PreUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    return true;
}

bool ModuleEditor::Update()
{

    int windowWidth, windowHeight;
    Application::GetInstance().window->GetWindowSize(windowWidth, windowHeight);

    bool windowResized = (windowWidth != lastWindowWidth || windowHeight != lastWindowHeight);
    lastWindowWidth = windowWidth;
    lastWindowHeight = windowHeight;

    float halfHeight = (windowHeight - 20) * 0.5f;

    ImGuiCond condition = windowResized ? ImGuiCond_Always : ImGuiCond_FirstUseEver;

    // Up left
    if (showConfiguration) {
        ImGui::SetNextWindowPos(ImVec2(windowWidth * 0.75f, 20 + halfHeight + (halfHeight * 0.25f)), condition);
        ImGui::SetNextWindowSize(ImVec2(windowWidth * 0.25f, halfHeight * 0.75f), condition);
        DrawConfigurationWindow();
    }

    // Down left
    if (showConsole) {
        ImGui::SetNextWindowPos(ImVec2(0, 20 + halfHeight + (halfHeight * 0.5f)), condition);
        ImGui::SetNextWindowSize(ImVec2(windowWidth * 0.75f, halfHeight * 0.5f), condition);
        DrawConsoleWindow();
    }

    // Down right
    if (showHierarchy) {
        ImGui::SetNextWindowPos(ImVec2(0, 20), condition);
        ImGui::SetNextWindowSize(ImVec2(windowWidth * 0.30f, halfHeight + (halfHeight * 0.5f)), condition);
        DrawHierarchyWindow();
    }

    // Up right
    if (showInspector) {
        ImGui::SetNextWindowPos(ImVec2(windowWidth * 0.75f, 20), condition);
        ImGui::SetNextWindowSize(ImVec2(windowWidth * 0.25f, halfHeight + (halfHeight * 0.25f)), condition);
        DrawInspectorWindow();
    }

    if (showAbout)
        DrawAboutWindow();

    return true;
}

bool ModuleEditor::PostUpdate()
{
    ShowMenuBar();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return true;
}

bool ModuleEditor::CleanUp()
{
    LOG_DEBUG("Cleaning up Editor");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    return true;
}

bool ModuleEditor::ShowMenuBar() {
    if (ImGui::BeginMainMenuBar()) {

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                Application::GetInstance().RequestExit();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Configuration", NULL, &showConfiguration);
            ImGui::MenuItem("Console", NULL, &showConsole);
            ImGui::MenuItem("Hierarchy", NULL, &showHierarchy);
            ImGui::MenuItem("Inspector", NULL, &showInspector);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("GameObject")) {
            if (ImGui::BeginMenu("Create Primitive")) {
                if (ImGui::MenuItem("Cube")) {
                    CreatePrimitiveGameObject("Cube", Primitives::CreateCube());
                }
                if (ImGui::MenuItem("Pyramid")) {
                    CreatePrimitiveGameObject("Pyramid", Primitives::CreatePyramid());
                }
                if (ImGui::MenuItem("Plane")) {
                    CreatePrimitiveGameObject("Plane", Primitives::CreatePlane());
                }
                if (ImGui::MenuItem("Sphere")) {
                    CreatePrimitiveGameObject("Sphere", Primitives::CreateSphere());
                }
                if (ImGui::MenuItem("Cylinder")) {
                    CreatePrimitiveGameObject("Cylinder", Primitives::CreateCylinder());
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("Documentation")) {
                SDL_OpenURL("https://github.com/Audra0000/Engine/blob/main/README.md"); 
            }
            if (ImGui::MenuItem("Report a Bug")) {
                SDL_OpenURL("https://github.com/Audra0000/Engine/issues");
            }
            if (ImGui::MenuItem("Download Latest")) {
                SDL_OpenURL("https://github.com/Audra0000/Engine/releases");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("About")) {
                showAbout = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    return true;
}

void ModuleEditor::DrawConfigurationWindow()
{
    ImGui::Begin("Configuration", &showConfiguration);

    // FPS Graph
    if (ImGui::CollapsingHeader("FPS")) 
    {
        DrawFPSGraph();
    }

    ImGui::Separator();

    // Window
    if (ImGui::CollapsingHeader("Window"))
    {
        DrawWindowSettings();
    }

    ImGui::Separator();

    // Hardware
    if (ImGui::CollapsingHeader("Hardware"))
    {
        DrawHardwareInfo();
    }

    ImGui::End();
}

void ModuleEditor::CreatePrimitiveGameObject(const std::string& name, Mesh mesh)
{
    GameObject* Object = new GameObject(name);
    ComponentMesh* meshComp = static_cast<ComponentMesh*>(Object->CreateComponent(ComponentType::MESH));

    // Seleccionar primitiva seg�n el nombre
    Mesh selectedMesh;

    if (name == "Cube")
        selectedMesh = Primitives::CreateCube();
    else if (name == "Pyramid")
        selectedMesh = Primitives::CreatePyramid();
    else if (name == "Plane")
        selectedMesh = Primitives::CreatePlane();
    else if (name == "Sphere")
        selectedMesh = Primitives::CreateSphere();
    else if (name == "Cylinder")
        selectedMesh = Primitives::CreateCylinder();
    else
        selectedMesh = mesh; 

    meshComp->SetMesh(selectedMesh);

    GameObject* root = Application::GetInstance().scene->GetRoot();
    root->AddChild(Object);

    LOG_CONSOLE("%s created", name.c_str());
    LOG_DEBUG("Primitive created: %s", name.c_str());
}

void ModuleEditor::DrawFPSGraph()
{
    fpsTimer += ImGui::GetIO().DeltaTime;

    if (fpsTimer >= 0.1f)
    {
        float fps = ImGui::GetIO().Framerate;
        fpsHistory.push_back(fps);

        if (fpsHistory.size() > maxFPSHistory)
        {
            fpsHistory.erase(fpsHistory.begin());
        }

        fpsTimer = 0.0f;
    }

    ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);

    if (!fpsHistory.empty())
    {
		// FPS graph
        ImGui::PlotLines("##FPS", fpsHistory.data(), (int)fpsHistory.size(), 0, nullptr, 0.0f, 200.0f, ImVec2(0, 80));
    }
}

void ModuleEditor::DrawHardwareInfo()
{
    // CPU
    ImGui::Text("CPU Cores: %d", SDL_GetNumLogicalCPUCores());
    // RAM
    ImGui::Text("System RAM: %d MB", SDL_GetSystemRAM());

#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc; // This is used to store memory
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
    {
        // workingSetSize gives used ram in bytes, but we want in mb so we divide it with (1024 * 1024) which is 1,048,576 bytes.
        SIZE_T memoryUsedMB = pmc.WorkingSetSize / (1024 * 1024);
        ImGui::Text("Memory Usage: %llu MB", memoryUsedMB);
    }
#endif

    // GPU
    ImGui::Text("GPU: %s", glGetString(GL_RENDERER));

    // Libraries versions
    ImGui::Separator();
    ImGui::Text("Software Versions:");

    // SDL
    int sdlVersion = SDL_GetVersion();
    int major = SDL_VERSIONNUM_MAJOR(sdlVersion);
    int minor = SDL_VERSIONNUM_MINOR(sdlVersion);
    int patch = SDL_VERSIONNUM_MICRO(sdlVersion);
    ImGui::BulletText("SDL3: %d.%d.%d", major, minor, patch);

	// OpenGL
    ImGui::BulletText("OpenGL: %s", glGetString(GL_VERSION));
	// ImGui
    ImGui::BulletText("ImGui: %s", IMGUI_VERSION);

	// DevIL
    ILint devilVersion = ilGetInteger(IL_VERSION_NUM);
    int devilMajor = devilVersion / 100;
    int devilMinor = (devilVersion / 10) % 10;
    int devilPatch = devilVersion % 10;
    ImGui::BulletText("DevIL: %d.%d.%d", devilMajor, devilMinor, devilPatch);
}

void ModuleEditor::DrawWindowSettings()
{
    int width, height;
    Application::GetInstance().window->GetWindowSize(width, height);

    ImGui::Text("Window Size: %dx%d", width, height);

    // Width slider
    int tempWidth = width;
    if (ImGui::InputInt("Width", &tempWidth, 10, 100))
    {
        if (tempWidth > 0)
        {
            SDL_SetWindowSize(Application::GetInstance().window->GetWindow(), tempWidth, height);
        }
    }

    // Height slider
    int tempHeight = height;
    if (ImGui::InputInt("Height", &tempHeight, 10, 100))
    {
        if (tempHeight > 0)
        {
            SDL_SetWindowSize(Application::GetInstance().window->GetWindow(), width, tempHeight);
        }
    }

    ImGui::Separator();

    // Fullscreen
    if (ImGui::Checkbox("Fullscreen", &fullscreen))
    {
        SDL_SetWindowFullscreen(Application::GetInstance().window->GetWindow(), fullscreen);
    }

    // Borderless
    static bool borderlessFullscreen = false;
    if (ImGui::Checkbox("Borderless Window", &borderlessFullscreen))
    {
        SDL_SetWindowBordered(Application::GetInstance().window->GetWindow(), !borderlessFullscreen);
    }

    ImGui::Separator();

    static bool resizable = true;
    if (ImGui::Checkbox("Resizable", &resizable))
    {
        SDL_SetWindowResizable(Application::GetInstance().window->GetWindow(), resizable);
    }

    ImGui::Separator();

    ImGui::Text("Resolutions:");

    if (ImGui::Button("Set 1920x1080"))
    {
        SDL_SetWindowSize(Application::GetInstance().window->GetWindow(), 1920, 1080);
    }
    ImGui::SameLine();
    if (ImGui::Button("Set 1280x720"))
    {
        SDL_SetWindowSize(Application::GetInstance().window->GetWindow(), 1280, 720);
    }
}

void ModuleEditor::DrawHierarchyWindow()
{
    ImGui::Begin("Hierarchy", &showHierarchy);

    GameObject* root = Application::GetInstance().scene->GetRoot();

    if (root != nullptr)
    {
        DrawGameObjectNode(root);
    }
    else
    {
        ImGui::TextDisabled("No scene loaded");
    }

    ImGui::End();
}

void ModuleEditor::DrawInspectorWindow()
{
    ImGui::Begin("Inspector", &showInspector);

    SelectionManager* selectionManager = Application::GetInstance().selectionManager;

    if (!selectionManager->HasSelection())
    {
        ImGui::TextDisabled("No GameObject selected");
        ImGui::End();
        return;
    }

    GameObject* selectedObject = selectionManager->GetSelectedObject();

    if (selectedObject == nullptr)
    {
        ImGui::TextDisabled("Invalid selection");
        ImGui::End();
        return;
    }

    ImGui::Text("GameObject: %s", selectedObject->GetName().c_str());
    ImGui::Separator();

    // Transforms
    Transform* transform = static_cast<Transform*>(selectedObject->GetComponent(ComponentType::TRANSFORM));

    if (transform != nullptr)
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            glm::vec3 position = transform->GetPosition();
            glm::vec3 rotation = transform->GetRotation();
            glm::vec3 scale = transform->GetScale();

            ImGui::PushItemWidth(80); 
            ImGui::BeginDisabled();

            // Position
            ImGui::Text("Position");
            ImGui::Text("X"); ImGui::SameLine(20);
            ImGui::DragFloat("##PosX", &position.x, 0.1f); ImGui::SameLine(120);
            ImGui::Text("Y"); ImGui::SameLine(130);
            ImGui::DragFloat("##PosY", &position.y, 0.1f); ImGui::SameLine(230);
            ImGui::Text("Z"); ImGui::SameLine(240);
            ImGui::DragFloat("##PosZ", &position.z, 0.1f);

            ImGui::Spacing();

            // Rotation
            ImGui::Text("Rotation");
            ImGui::Text("X"); ImGui::SameLine(20);
            ImGui::DragFloat("##RotX", &rotation.x, 0.1f); ImGui::SameLine(120);
            ImGui::Text("Y"); ImGui::SameLine(130);
            ImGui::DragFloat("##RotY", &rotation.y, 0.1f); ImGui::SameLine(230);
            ImGui::Text("Z"); ImGui::SameLine(240);
            ImGui::DragFloat("##RotZ", &rotation.z, 0.1f);

            ImGui::Spacing();

            // Scale
            ImGui::Text("Scale");
            ImGui::Text("X"); ImGui::SameLine(20);
            ImGui::DragFloat("##ScaleX", &scale.x, 0.1f); ImGui::SameLine(120);
            ImGui::Text("Y"); ImGui::SameLine(130);
            ImGui::DragFloat("##ScaleY", &scale.y, 0.1f); ImGui::SameLine(230);
            ImGui::Text("Z"); ImGui::SameLine(240);
            ImGui::DragFloat("##ScaleZ", &scale.z, 0.1f);

            ImGui::EndDisabled();
            ImGui::PopItemWidth();
        }
    }
    // Mesh
    ComponentMesh* meshComp = static_cast<ComponentMesh*>(selectedObject->GetComponent(ComponentType::MESH));

    if (meshComp != nullptr && meshComp->HasMesh())
    {
        if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const Mesh& mesh = meshComp->GetMesh();

            // Mesh information
            ImGui::Text("Vertices: %d", (int)mesh.vertices.size());
            ImGui::Text("Indices: %d", (int)mesh.indices.size());
            ImGui::Text("Triangles: %d", (int)mesh.indices.size() / 3);

            ImGui::Separator();

            // Normals visualization
            ImGui::Text("Normals Visualization:");

            if (ImGui::Checkbox("Show Vertex Normals", &showVertexNormals))
            {
                LOG_DEBUG("Vertex normals visualization: %s", showVertexNormals ? "ON" : "OFF");
            }

            if (ImGui::Checkbox("Show Face Normals", &showFaceNormals))
            {
                LOG_DEBUG("Face normals visualization: %s", showFaceNormals ? "ON" : "OFF");
            }
        }
    }
    ComponentMaterial* materialComp = static_cast<ComponentMaterial*>(selectedObject->GetComponent(ComponentType::MATERIAL));

    if (materialComp != nullptr)
    {
        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
			// Texture information
            ImGui::Text("Texture Information:");

            std::string texturePath = materialComp->GetTexturePath();
            if (texturePath.empty() || texturePath == "[Checkerboard Pattern]")
            {
                ImGui::Text("Path: Checkerboard");
            }
            else
            {
                ImGui::Text("Path: %s", texturePath.c_str());
            }

            ImGui::Text("Size: %d x %d pixels", materialComp->GetTextureWidth(), materialComp->GetTextureHeight());

            ImGui::Separator();

            if (materialComp->HasOriginalTexture())
            {
                ImGui::Separator();
                ImGui::Text("Original Texture:");
                ImGui::Text("Path: %s", materialComp->GetOriginalTexturePath().c_str());
            }

            ImGui::Separator();

            ImGui::BeginGroup();

            if (ImGui::Button("Apply Checkerboard"))
            {
                materialComp->CreateCheckerboardTexture();
                LOG_DEBUG("Applied checkerboard texture to: %s", selectedObject->GetName().c_str());
                LOG_CONSOLE("Checkerboard texture applied to %s", selectedObject->GetName().c_str());
            }

			if (ImGui::IsItemHovered()) // Helper tooltip
            {
                ImGui::SetTooltip("Applies the default black and white checkerboard pattern to this GameObject");
            }

            if (materialComp->HasOriginalTexture())
            {
                ImGui::SameLine();

                if (ImGui::Button("Restore Original"))
                {
                    materialComp->RestoreOriginalTexture();
                    LOG_DEBUG("Restored original texture to: %s", selectedObject->GetName().c_str());
                    LOG_CONSOLE("Original texture restored to %s", selectedObject->GetName().c_str());
                }

                if (ImGui::IsItemHovered()) // Helper tooltip
                {
                    ImGui::SetTooltip("Restores the original texture that was previously loaded");
                }
            }

            ImGui::EndGroup();
        }
    }
    else
    {
        if (meshComp != nullptr && meshComp->HasMesh())
        {
            if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("No material component");
            }
        }
    }
    ImGui::End();
}

void ModuleEditor::DrawConsoleWindow()
{
    ImGui::Begin("Console", &showConsole);

    if (ImGui::Button("Clear"))
    {
        ConsoleLog::GetInstance().Clear();
    }

    ImGui::SameLine();
    ImGui::Checkbox("Library Info", &showLibraryInfo);
    ImGui::SameLine();
    ImGui::Checkbox("Errors", &showErrors);
    ImGui::SameLine();
    ImGui::Checkbox("Warnings", &showWarnings);
    ImGui::SameLine();
    ImGui::Checkbox("Loading", &showLoading);
    ImGui::SameLine();
    ImGui::Checkbox("Success", &showSuccess);
    ImGui::SameLine();
    ImGui::Checkbox("Info", &showInfo);
    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &autoScroll);

    ImGui::Separator();

    ImVec2 availableSpace = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("Scrolling", availableSpace, true, ImGuiWindowFlags_HorizontalScrollbar);

    const std::vector<std::string>& logs = ConsoleLog::GetInstance().GetLogs();

    for (const auto& log : logs)
    {
        ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White
        bool isError = false;
        bool isWarning = false;
        bool isInfo = false;
        bool isSuccess = false;
        bool isLoading = false;
        bool isLibraryInfo = false;

        if (log.find("DevIL") != std::string::npos || log.find("SDL3") != std::string::npos || log.find("OpenGL") != std::string::npos || log.find("ASSIMP") != std::string::npos || log.find("Mesh processed") != std::string::npos)
        {
            color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
            isLibraryInfo = true;
        }
        else if (log.find("ERROR") != std::string::npos || log.find("Failed") != std::string::npos)
        {
            color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); // Red
            isError = true;
        }
        else if (log.find("WARNING") != std::string::npos || log.find("Corrupt") != std::string::npos)
        {
            color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
            isWarning = true;
        }
        else if (log.find("Loading") != std::string::npos || log.find("Initializing") != std::string::npos)
        {
            color = ImVec4(1.0f, 0.7f, 0.3f, 1.0f); // Orange
            isLoading = true;
        }
        else if (log.find("ready") != std::string::npos || log.find("loaded") != std::string::npos || log.find("initialized") != std::string::npos)
        {
            color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f); // Green
            isSuccess = true;
        }
        else isInfo = true;

        if (isLibraryInfo)
        {
            if (!showLibraryInfo)
                continue;
        }
        else if ((isError && !showErrors) || (isWarning && !showWarnings) || (isSuccess && !showSuccess) || (isInfo && !showInfo) || (isLoading && !showLoading))
        {
            continue; 
        }

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextWrapped("%s", log.c_str());
        ImGui::PopStyleColor();
    }

    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    if (scrollToBottom)
    {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;
    }

    ImGui::EndChild();
    ImGui::End();
}

void ModuleEditor::DrawAboutWindow()
{
    ImGui::Begin("About", &showAbout);

    ImGui::Text("Engine Name: Engine");
    ImGui::Text("Version: 0.1.0");

    ImGui::Separator();

    ImGui::Text("Team Members:");
    ImGui::BulletText("Haosheng Li");
    ImGui::BulletText("");

    ImGui::Separator();

    ImGui::Text("Libraries Used:");

    // SDL
    int sdlVersion = SDL_GetVersion();
    int major = SDL_VERSIONNUM_MAJOR(sdlVersion);
    int minor = SDL_VERSIONNUM_MINOR(sdlVersion);
    int patch = SDL_VERSIONNUM_MICRO(sdlVersion);
    ImGui::BulletText("SDL3: %d.%d.%d", major, minor, patch);

    // OpenGL
    ImGui::BulletText("OpenGL: %s", glGetString(GL_VERSION));

    // ImGui
    ImGui::BulletText("ImGui: %s", IMGUI_VERSION);

    // DevIL
    ILint devilVersion = ilGetInteger(IL_VERSION_NUM);
    int devilMajor = devilVersion / 100;
    int devilMinor = (devilVersion / 10) % 10;
    int devilPatch = devilVersion % 10;
    ImGui::BulletText("DevIL: %d.%d.%d", devilMajor, devilMinor, devilPatch);

    ImGui::BulletText("Glad");

    ImGui::BulletText("Glm");

    ImGui::BulletText("Assimp");

    ImGui::Separator();

    ImGui::TextWrapped("MIT License");
    ImGui::Spacing();
    ImGui::TextWrapped("Copyright (c) 2025 Engine");
    ImGui::Spacing();
    ImGui::TextWrapped(
        "Permission is hereby granted, free of charge, to any person obtaining a copy "
        "of this software and associated documentation files (the \"Software\"), to deal "
        "in the Software without restriction, including without limitation the rights "
        "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
        "copies of the Software, and to permit persons to whom the Software is "
        "furnished to do so, subject to the following conditions:"
    );
    ImGui::Spacing();
    ImGui::TextWrapped(
        "The above copyright notice and this permission notice shall be included in all "
        "copies or substantial portions of the Software."
    );
    ImGui::Spacing();
    ImGui::TextWrapped(
        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
        "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE "
        "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
        "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
        "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
        "SOFTWARE."
    );

    ImGui::End();
}

void ModuleEditor::DrawGameObjectNode(GameObject* gameObject)
{
    if (gameObject == nullptr)
        return;

    const std::vector<GameObject*>& children = gameObject->GetChildren();
    bool hasChildren = !children.empty();

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow; //  | ImGuiTreeNodeFlags_OpenOnDoubleClick

	// Handle selected game object
    SelectionManager* selectionManager = Application::GetInstance().selectionManager;
    if (selectionManager->IsSelected(gameObject))
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    if (!hasChildren)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // =================================================== Handle renaming =====================================================
    bool isRenaming = (renamingObject == gameObject);

    if (isRenaming)
    {
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("##", renameBuffer, sizeof(renameBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))         // Show input text field for renaming
        {
            if (strlen(renameBuffer) > 0)
            {
                gameObject->SetName(renameBuffer);
                LOG_DEBUG("GameObject renamed to: %s", renameBuffer);
            }
            renamingObject = nullptr;
        }

		if (!ImGui::IsItemActive() && (ImGui::IsMouseClicked(0) || ImGui::IsKeyPressed(ImGuiKey_Escape))) // Clicked outside or pressed Escape
        {
            renamingObject = nullptr;
        }

    }
    else
    {
	// ============================================ Display GameObject Node ===================================================
        // Display the node
        bool nodeOpen = ImGui::TreeNodeEx(gameObject, nodeFlags, "%s", gameObject->GetName().c_str());
        // Handle selection
        if (ImGui::IsItemClicked())
        {
            const bool* keys = SDL_GetKeyboardState(NULL);
            bool shiftPressed = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];

            if (shiftPressed)
            {
                // Shift+ click: Multi select
                selectionManager->ToggleSelection(gameObject);
            }
            else
            {
                if (hasChildren)
                {
                    // Select all children
                    selectionManager->ClearSelection();
                    SelectGameObjectAndChildren(gameObject);
                }
                else
                {
                    // Click 
                    selectionManager->SetSelectedObject(gameObject);
                }
            }
        }

        // Handle renaming on double click
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) // Mouse over and double clicked
        {
            renamingObject = gameObject;
			strncpy(renameBuffer, gameObject->GetName().c_str(), sizeof(renameBuffer) - 1); // Copy current name to buffer
        }

        // Recursively draw children
        if (nodeOpen && hasChildren)
        {
            for (GameObject* child : children)
            {
                DrawGameObjectNode(child);
            }
            ImGui::TreePop();
        }
    }
}

void ModuleEditor::SelectGameObjectAndChildren(GameObject* gameObject)
{
    if (gameObject == nullptr)
        return;

    // Add GameObject to selection
    SelectionManager* selectionManager = Application::GetInstance().selectionManager;
    selectionManager->AddToSelection(gameObject);

    // Recursively add all children
    const std::vector<GameObject*>& children = gameObject->GetChildren();
    for (GameObject* child : children)
    {
        SelectGameObjectAndChildren(child);
    }
}