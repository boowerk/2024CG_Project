#include "PointLight.h"

#include <iostream>

#include "imgui.h"
#include "ImGuizmo.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Shader.h"

PointLight::PointLight(GLfloat ambient, GLfloat diffuse, glm::vec4 color, glm::vec3 position, GLfloat constant, GLfloat linear, GLfloat exponent):
	Light(ambient, diffuse, color),
	position(position),
	constant(constant),
	linear(linear),
	exponent(exponent)
{
	modelMat = glm::translate(glm::mat4(1.f), position);

	// mesh setup
	std::vector<unsigned int> pointLightIndices = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	std::vector<Vertex> pointLightVertices = {
		Vertex(-0.05f, 0.05f, -0.05f) ,
		Vertex(-0.05f, -0.05f, -0.05f),
		Vertex(0.05f, 0.05f, -0.05f),
		Vertex(0.05f, -0.05f, -0.05f),

		Vertex(-0.05f, 0.05f, 0.05f),
		Vertex(0.05f, 0.05f, 0.05f),
		Vertex(-0.05f, -0.05f, 0.05f),
		Vertex(0.05f, -0.05f, 0.05f),
	};

	pointLightMesh = new Mesh();
	pointLightMesh->CreateMesh(pointLightVertices, pointLightIndices);
}

void PointLight::ShowProperties()
{
	// Transform
	ImGui::Text("Transform");

	ImGui::InputFloat3("Position", &position[0]);
	ImGui::InputFloat3("Color", &color[0]);
	ImGui::SliderFloat("Ambient", GetAmbientIntensity(), 0.f, 5.f);
	ImGui::SliderFloat("Diffuse", GetDiffuseIntensity(), 0.f, 5.f);

	// Range
	ImGui::Text("Decreasement");
	ImGui::SliderFloat("Constant", &constant, 0.f, 1.f);
	ImGui::SliderFloat("Linear", &linear, 0.f, 1.f);
	ImGui::SliderFloat("Exponent", &exponent, 0.f, 1.f);
}

glm::mat4 PointLight::GetModelMat()
{
	return modelMat = glm::translate(glm::mat4(1.f), position);
}

void PointLight::UpdateTransform(glm::mat4 newModelMat)
{
	glm::vec3 translation, rotation, scale;
	ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newModelMat), &translation[0], &rotation[0], &scale[0]);

	position = translation;
}

void PointLight::DrawPointLight(glm::mat4 viewMat, glm::mat4 projMat, Shader* pointLightShader)
{
	glm::mat4 PVM = projMat * viewMat * modelMat; 

	pointLightShader->setVec4("color", color);
	pointLightShader->setMat4("PVM", PVM);

	pointLightMesh->RenderMesh();
}
