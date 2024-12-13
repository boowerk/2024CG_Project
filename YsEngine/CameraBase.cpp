#include "CameraBase.h"

CameraBase::CameraBase(glm::vec3 startPosition, GLfloat startPitch, GLfloat startYaw)
{
	position = startPosition;
	worldUp = glm::vec3(0.f, 1.f, 0.f);
	pitch = startPitch;
	yaw = startYaw;

	nearClippingPlane = 0.1f;
	farClippingPlane = 100000.0f;
}

glm::mat4 CameraBase::GetViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 CameraBase::GetProjectionMatrix(GLfloat width, GLfloat height)
{
	return glm::perspective(glm::radians(45.0f),
		width / height, nearClippingPlane, farClippingPlane);
}

glm::vec3 CameraBase::GetPosition()
{
	return position;
}

CameraBase::~CameraBase()
{
}

void CameraBase::Update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}
