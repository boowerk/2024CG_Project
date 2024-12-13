#pragma once

#include "CameraBase.h"
#include "Entity.h"

class Window;

class FreeCamera : public CameraBase
{
public:
	FreeCamera(glm::vec3 startPosition, GLfloat moveSpeed, GLfloat turnSpeed, 
		GLfloat startPitch = 0.f, GLfloat startYaw= -90.f);

	virtual void KeyControl(bool* keys, GLfloat deltaTime) override;
	virtual void MouseControl(GLfloat xChange, GLfloat yChange) override;
	virtual void ScrollControl(GLfloat scrollY) override;
	void SetPosition(glm::vec3 pos) { position = pos; }

	~FreeCamera();

private:
	GLfloat moveSpeed;
	GLfloat turnSpeed;

	bool canMove;
};

