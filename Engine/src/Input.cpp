#include "Input.h"
#include "Window.h"
#include "Application.h"
#include <iostream>
#include "Camera.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "GameObject.h"
#include "Transform.h"
#include "ComponentMesh.h"
#include <limits>

#define MAX_KEYS 300

Input::Input() : Module(), droppedFile(false), droppedFilePath(""), droppedFileType(DROPPED_NONE)
{
	keyboard = new KeyState[MAX_KEYS];
	// reserve memory
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouseButtons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

Input::~Input()
{
	delete[] keyboard;
}

// Called before render is available
bool Input::Awake()
{
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool Input::Start()
{
	return true;
}

// Called each loop iteration
bool Input::PreUpdate()
{
	static SDL_Event event;
	const bool* keys = SDL_GetKeyboardState(NULL);

	// Reset dropped file flag each frame
	droppedFile = false;

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i])
		{
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}
	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouseButtons[i] == KEY_DOWN)
			mouseButtons[i] = KEY_REPEAT;
		if (mouseButtons[i] == KEY_UP)
			mouseButtons[i] = KEY_IDLE;
	}

	ImGuiIO& io = ImGui::GetIO();
	bool imguiWantCaptureMouse = io.WantCaptureMouse;
	bool imguiWantCaptureKeyboard = io.WantCaptureKeyboard;

	while (SDL_PollEvent(&event))
	{
		// Imgui event
		ImGui_ImplSDL3_ProcessEvent(&event);

		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			windowEvents[WE_QUIT] = true;
			break;
		case SDL_EVENT_WINDOW_HIDDEN:
		case SDL_EVENT_WINDOW_MINIMIZED:
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			windowEvents[WE_HIDE] = true;
			break;
		case SDL_EVENT_WINDOW_SHOWN:
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
		case SDL_EVENT_WINDOW_MAXIMIZED:
		case SDL_EVENT_WINDOW_RESTORED:
			windowEvents[WE_SHOW] = true;
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			if (!imguiWantCaptureMouse)
			{
				mouseButtons[event.button.button - 1] = KEY_DOWN;
				Camera* camera = Application::GetInstance().renderer->GetCamera();

				if (event.button.button == SDL_BUTTON_RIGHT)
				{
					camera->ResetMouseInput();
				}
				else if (event.button.button == SDL_BUTTON_LEFT)
				{
					camera->ResetOrbitInput();

					// Object selection: only when ALT is not pressed (ALT+LMB is reserved for orbit)
					if (!keys[SDL_SCANCODE_LALT] && !keys[SDL_SCANCODE_RALT])
					{
						// Get window dimensions and mouse position
						int screenWidth, screenHeight;
						SDL_GetWindowSize(Application::GetInstance().window.get()->GetWindow(),
							&screenWidth, &screenHeight);

						float mouseXf, mouseYf;
						SDL_GetMouseState(&mouseXf, &mouseYf);

						int scale = Application::GetInstance().window.get()->GetScale();
						int mouseX = static_cast<int>(mouseXf / scale);
						int mouseY = static_cast<int>(mouseYf / scale);

						// Create ray from camera through mouse position
						glm::vec3 rayOrigin = camera->GetPosition();
						glm::vec3 rayDir = camera->ScreenToWorldRay(mouseX, mouseY,
							screenWidth / scale,
							screenHeight / scale);

						// Find closest object intersected by ray
						GameObject* root = Application::GetInstance().scene->GetRoot();
						float minDist = std::numeric_limits<float>::max();
						GameObject* clicked = FindClosestObjectToRay(root, rayOrigin, rayDir, minDist);

						bool shiftPressed = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];

						if (clicked)
						{
							if (shiftPressed)
							{
								Application::GetInstance().selectionManager->ToggleSelection(clicked);
							}
							else
							{
								Application::GetInstance().selectionManager->SetSelectedObject(clicked);
							}
						}
						else
						{
							// Clicked on empty space: clear selection unless Shift is held
							if (!shiftPressed)
							{
								Application::GetInstance().selectionManager->ClearSelection();
							}
						}
					}
				}
				else if (event.button.button == SDL_BUTTON_MIDDLE)
				{
					camera->ResetPanInput();
				}
			}
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
			if (!imguiWantCaptureMouse)
			{
				mouseButtons[event.button.button - 1] = KEY_UP;
			}
			break;
		case SDL_EVENT_MOUSE_MOTION:
		{
			if (!imguiWantCaptureMouse)
			{
				int scale = Application::GetInstance().window.get()->GetScale();
				mouseMotionX = static_cast<int>(event.motion.xrel / scale);
				mouseMotionY = static_cast<int>(event.motion.yrel / scale);

				mouseX = static_cast<int>(event.motion.x / scale);
				mouseY = static_cast<int>(event.motion.y / scale);

				float mouseXf = static_cast<float>(event.motion.x) / static_cast<float>(scale);
				float mouseYf = static_cast<float>(event.motion.y) / static_cast<float>(scale);

				Camera* camera = Application::GetInstance().renderer->GetCamera();

				// Alt + Click Izquierdo - orbit
				if ((keys[SDL_SCANCODE_LALT] || keys[SDL_SCANCODE_RALT]) &&
					(mouseButtons[SDL_BUTTON_LEFT - 1] == KEY_REPEAT || mouseButtons[SDL_BUTTON_LEFT - 1] == KEY_DOWN))
				{
					camera->HandleOrbitInput(mouseXf, mouseYf);
				}
				// Click Medio - Pan
				else if (mouseButtons[SDL_BUTTON_MIDDLE - 1] == KEY_REPEAT || mouseButtons[SDL_BUTTON_MIDDLE - 1] == KEY_DOWN)
				{
					camera->HandlePanInput(static_cast<float>(mouseMotionX), static_cast<float>(mouseMotionY));
				}
				// Click Derecho - Look around 
				else if (mouseButtons[SDL_BUTTON_RIGHT - 1] == KEY_REPEAT || mouseButtons[SDL_BUTTON_RIGHT - 1] == KEY_DOWN)
				{
					camera->HandleMouseInput(mouseXf, mouseYf);
				}
			}
		}
		break;

		// Drag and Drop - FBX& textures
		case SDL_EVENT_DROP_FILE:
			if (event.drop.data != nullptr)
			{
				droppedFilePath = event.drop.data;
				droppedFile = true;
				std::cout << "File dropped: " << droppedFilePath << std::endl;

				// Determine file type
				if (droppedFilePath.size() >= 4)
				{
					std::string extension = droppedFilePath.substr(droppedFilePath.size() - 4);
					for (char& c : extension) c = tolower(c);

					if (extension == ".fbx")
					{
						droppedFileType = DROPPED_FBX;
					}
					else if (extension == ".png" || extension == ".dds")
					{
						droppedFileType = DROPPED_TEXTURE;
					}
					else
					{
						droppedFileType = DROPPED_NONE;
					}
				}
			}
			break;

		case SDL_EVENT_MOUSE_WHEEL:
		{
			if (!imguiWantCaptureMouse)
			{
				Camera* camera = Application::GetInstance().renderer->GetCamera();
				camera->HandleScrollInput(static_cast<float>(event.wheel.y));
			}
		}
		break;
		}
	}

	// Tecla F - Focus en geometria (falta implementar logica seleccion)
	//if (keyboard[SDL_SCANCODE_F] == KEY_DOWN)
	//{
	//	Camera* camera = Application::GetInstance().renderer->GetCamera();
	//	glm::vec3 selectedObjectPosition(0.0f, 0.0f, 0.0f);
	//	float selectedObjectRadius = 1.0f; // Radio aproximado del objeto
	//	camera->FocusOnTarget(selectedObjectPosition, selectedObjectRadius);
	//}

	Camera* camera = Application::GetInstance().renderer->GetCamera();
	const float cameraBaseSpeed = 2.5f;
	float speedMultiplier = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT] ? 2.0f : 1.0f;
	float cameraSpeed = cameraBaseSpeed * speedMultiplier * Application::GetInstance().time->GetDeltaTime();
	// Only active when you press right-click (WASD movement)
	if (!io.WantCaptureKeyboard && (mouseButtons[SDL_BUTTON_RIGHT - 1] == KEY_REPEAT || mouseButtons[SDL_BUTTON_RIGHT - 1] == KEY_DOWN))
	{
		glm::vec3 cameraFront = camera->GetFront();
		glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, camera->GetUp()));
		glm::vec3 cameraUp = camera->GetUp();

		// WASD - Movimiento horizontal
		if (keys[SDL_SCANCODE_W])
			camera->SetPosition(camera->GetPosition() + cameraSpeed * cameraFront);
		if (keys[SDL_SCANCODE_S])
			camera->SetPosition(camera->GetPosition() - cameraSpeed * cameraFront);
		if (keys[SDL_SCANCODE_A])
			camera->SetPosition(camera->GetPosition() - cameraRight * cameraSpeed);
		if (keys[SDL_SCANCODE_D])
			camera->SetPosition(camera->GetPosition() + cameraRight * cameraSpeed);
	}
	glm::vec3 cameraUp = camera->GetUp();
	// Up
	if (keys[SDL_SCANCODE_SPACE])
		camera->SetPosition(camera->GetPosition() + cameraUp * cameraSpeed);

	// Down
	if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL])
		camera->SetPosition(camera->GetPosition() - cameraUp * cameraSpeed);

	return true;
}

// Called before quitting
bool Input::CleanUp()
{
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

bool Input::GetWindowEvent(EventWindow ev)
{
	return windowEvents[ev];
}
// Ray-AABB intersection using slab method
bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
	const glm::vec3& aabbMin, const glm::vec3& aabbMax,
	float& distance)
{
	// Compute inverse direction to avoid division in loop
	glm::vec3 invDir;
	invDir.x = (std::abs(rayDir.x) > 0.0001f) ? 1.0f / rayDir.x : std::numeric_limits<float>::max();
	invDir.y = (std::abs(rayDir.y) > 0.0001f) ? 1.0f / rayDir.y : std::numeric_limits<float>::max();
	invDir.z = (std::abs(rayDir.z) > 0.0001f) ? 1.0f / rayDir.z : std::numeric_limits<float>::max();

	glm::vec3 t0 = (aabbMin - rayOrigin) * invDir;
	glm::vec3 t1 = (aabbMax - rayOrigin) * invDir;

	glm::vec3 tmin = glm::min(t0, t1);
	glm::vec3 tmax = glm::max(t0, t1);

	float tNear = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
	float tFar = glm::min(glm::min(tmax.x, tmax.y), tmax.z);

	if (tNear > tFar || tFar < 0.0f)
	{
		return false;
	}

	distance = (tNear > 0.0f) ? tNear : tFar;
	return true;
}

// Recursively find closest object intersected by ray
GameObject* FindClosestObjectToRay(GameObject* obj, const glm::vec3& rayOrigin, const glm::vec3& rayDir, float& minDist)
{
	if (!obj || !obj->IsActive()) return nullptr;

	GameObject* closest = nullptr;

	ComponentMesh* mesh = static_cast<ComponentMesh*>(obj->GetComponent(ComponentType::MESH));
	if (mesh && mesh->IsActive() && mesh->HasMesh())
	{
		Transform* t = static_cast<Transform*>(obj->GetComponent(ComponentType::TRANSFORM));
		if (t)
		{
			glm::vec3 localMin = mesh->GetAABBMin();
			glm::vec3 localMax = mesh->GetAABBMax();

			glm::mat4 globalMatrix = t->GetGlobalMatrix();

			// Transform AABB to world space by transforming all 8 corners
			glm::vec3 corners[8] = {
				glm::vec3(globalMatrix * glm::vec4(localMin.x, localMin.y, localMin.z, 1.0f)),
				glm::vec3(globalMatrix * glm::vec4(localMax.x, localMin.y, localMin.z, 1.0f)),
				glm::vec3(globalMatrix * glm::vec4(localMin.x, localMax.y, localMin.z, 1.0f)),
				glm::vec3(globalMatrix * glm::vec4(localMax.x, localMax.y, localMin.z, 1.0f)),
				glm::vec3(globalMatrix * glm::vec4(localMin.x, localMin.y, localMax.z, 1.0f)),
				glm::vec3(globalMatrix * glm::vec4(localMax.x, localMin.y, localMax.z, 1.0f)),
				glm::vec3(globalMatrix * glm::vec4(localMin.x, localMax.y, localMax.z, 1.0f)),
				glm::vec3(globalMatrix * glm::vec4(localMax.x, localMax.y, localMax.z, 1.0f))
			};

			// Recompute axis-aligned bounding box in world space
			glm::vec3 worldMin = corners[0];
			glm::vec3 worldMax = corners[0];

			for (int i = 1; i < 8; ++i)
			{
				worldMin = glm::min(worldMin, corners[i]);
				worldMax = glm::max(worldMax, corners[i]);
			}

			float dist;
			if (RayIntersectsAABB(rayOrigin, rayDir, worldMin, worldMax, dist))
			{
				if (dist < minDist)
				{
					minDist = dist;
					closest = obj;
					LOG("Ray hit '%s' at distance %.2f", obj->GetName().c_str(), dist);
				}
			}
		}
	}

	// Recursively check children
	for (GameObject* child : obj->GetChildren())
	{
		GameObject* childResult = FindClosestObjectToRay(child, rayOrigin, rayDir, minDist);
		if (childResult) closest = childResult;
	}

	return closest;
}