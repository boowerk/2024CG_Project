#include "DirectionalLight.h"

#include "imgui.h"
#include "ImGuizmo.h"

void DirectionalLight::ShowProperties()
{
	// Directional Light
	ImGui::Text("DirectionalLight");

	ImGui::InputFloat3("Direction", &direction[0]);
	ImGui::InputFloat3("Color", &color[0]);
	ImGui::SliderFloat("Ambient", GetAmbientIntensity(), 0.f, 5.f);
	ImGui::SliderFloat("Diffuse", GetDiffuseIntensity(), 0.f, 5.f);
}

glm::mat4 DirectionalLight::GetModelMat()
{
	return modelMat = glm::translate(glm::mat4(1.f), direction);
}

void DirectionalLight::UpdateTransform(glm::mat4 newModelMat)
{
	glm::vec3 translation, rotation, scale;
	ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newModelMat), &translation[0], &rotation[0], &scale[0]);

	direction = translation;
}
