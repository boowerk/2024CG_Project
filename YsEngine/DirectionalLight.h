#pragma once
#include "Light.h"
class DirectionalLight : public Light
{
public:
	DirectionalLight(GLfloat ambient, GLfloat diffuse, glm::vec4 color, glm::vec3 direction) :
		Light(ambient, diffuse, color),
		direction(direction)
	{}

	virtual std::string GetName() override { return "DirectionalLight"; }
	virtual void ShowProperties() override;
	virtual glm::mat4 GetModelMat() override;
	virtual void UpdateTransform(glm::mat4 newModelMat) override;

	glm::vec3 direction;
};

