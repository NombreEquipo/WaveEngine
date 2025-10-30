#pragma once
#include <glm/glm.hpp>
#include "glm/gtc/quaternion.hpp"

class Camera
{
public:
	Camera();
	~Camera();

	void Update();

	const glm::mat4& GetViewMatrix() const { return viewMatrix; }
	const glm::mat4& GetProjectionMatrix() const { return projectionMatrix; }

	glm::vec3 GetPosition() const { return cameraPos; }
	glm::vec3 GetFront() const { return cameraFront; }
	glm::vec3 GetUp() const { return cameraUp; }
	float GetFov() const { return fov; }

	void SetPosition(const glm::vec3& newPosition) { cameraPos = newPosition; }
	void SetAspectRatio(float aspectRatio);  

	void HandleMouseInput(float xpos, float ypos);
	void HandleScrollInput(float yoffset);
	void ResetMouseInput() { firstMouse = true; }

	// for Unity controls
	void HandleOrbitInput(float xpos, float ypos);
	void HandlePanInput(float xoffset, float yoffset);
	void FocusOnTarget(const glm::vec3& targetPosition, float targetRadius = 1.0f);
	void SetOrbitTarget(const glm::vec3& target) { orbitTarget = target; }
	glm::vec3 GetOrbitTarget() const { return orbitTarget; }
	void ResetOrbitInput() { firstOrbit = true; }
	void ResetPanInput() { firstPan = true; }
	glm::vec3 ScreenToWorldRay(int mouseX, int mouseY, int screenWidth, int screenHeight) const;

private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	float yaw;
	float pitch;
	float lastX;
	float lastY;
	bool firstMouse;
	float fov;
	float aspectRatio;  

	// Variables for orbit
	glm::vec3 orbitTarget;
	float orbitDistance;
	float lastOrbitX;
	float lastOrbitY;
	bool firstOrbit;

	// Variables for pan
	float lastPanX;
	float lastPanY;
	bool firstPan;

	void UpdateCameraVectors();
	void ProcessMouseMovement(float xoffset, float yoffset);
	void UpdateOrbitPosition();
	void UpdateProjectionMatrix();  
};