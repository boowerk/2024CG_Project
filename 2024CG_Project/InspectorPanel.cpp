#include "InspectorPanel.h"

#include "imgui.h"

#include "Model.h"
#include "DirectionalLight.h"
#include "Material.h"

void InspectorPanel::Update()
{
	ImGui::Begin("Inspector");

	if(currEntity)
		currEntity->ShowProperties();

	ImGui::End();
}

void InspectorPanel::SetEntity(Entity* e)
{
	currEntity = e;
}

InspectorPanel::~InspectorPanel()
{
}
