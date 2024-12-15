#pragma once
#include "Light.h"

class Mesh;
class Shader;

class PointLight : public Light
{
public:
	PointLight(GLfloat ambient, GLfloat diffuse, glm::vec4 color, glm::vec3 position, GLfloat constant, GLfloat linear, GLfloat exponent);

	virtual std::string GetName() override { return "PointLight"; }
	virtual void ShowProperties() override;
	virtual glm::mat4 GetModelMat() override;
	virtual void UpdateTransform(glm::mat4 newModelMat) override;

	void DrawPointLight(glm::mat4 viewMat, glm::mat4 projMat, Shader* pointLightShader);

	glm::vec3 position;

	// attenuation
	GLfloat constant;
	GLfloat linear;
	GLfloat exponent;

	Mesh* pointLightMesh;

	GLuint loc_PVM;
};

