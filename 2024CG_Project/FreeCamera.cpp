#include "FreeCamera.h"

#include <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Window.h"
#include "imgui.h"

FreeCamera::FreeCamera(glm::vec3 startPosition, GLfloat moveSpeed, GLfloat turnSpeed,
					GLfloat startPitch, GLfloat startYaw) : CameraBase(startPosition, startPitch, startYaw)
{
	this->moveSpeed = moveSpeed;
	this->turnSpeed = turnSpeed;

	canMove = false;
}


void FreeCamera::KeyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_W])
		position += front * velocity;

	if (keys[GLFW_KEY_S])
		position -= front * velocity;

	if (keys[GLFW_KEY_A])
		position -= right * velocity;

	if (keys[GLFW_KEY_D])
		position += right * velocity;

	if (keys[GLFW_KEY_Q])
		position -= up * velocity;

	if (keys[GLFW_KEY_E])
		position += up * velocity;
}

void FreeCamera::MouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
		pitch = 89.0f;

	if (pitch < -89.0f)
		pitch = -89.0f;
}

void FreeCamera::ScrollControl(GLfloat scrollY)
{
	moveSpeed += scrollY * 5.f;
	moveSpeed = std::max(0.1f, moveSpeed);
}

FreeCamera::~FreeCamera()
{
}
