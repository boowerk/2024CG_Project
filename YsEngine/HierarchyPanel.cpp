#include "HierarchyPanel.h"

#include "imgui.h"
#include "Entity.h"
#include "InspectorPanel.h"
#include "ScenePanel.h"

#include <string>

HierarchyPanel::HierarchyPanel(std::vector<Entity*>& entityList, ScenePanel* sp)
	: entityList(entityList), selection(nullptr)
{
	inspectorPanel = new InspectorPanel();
	scenePanel = sp;
}

void HierarchyPanel::Update()
{
	ImGui::Begin("Hierarchy");

	for (auto entity : entityList)
		DrawEntityNode(entity);

	ImGui::End();

	inspectorPanel->Update();
}

void HierarchyPanel::UpdateEntityList(std::vector<Entity*>& entityList)
{
	this->entityList = entityList;
}

void HierarchyPanel::DrawEntityNode(Entity* entity)
{
	const auto& name = entity->GetName();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | (entity == selection ? ImGuiTreeNodeFlags_Selected : 0);
	bool opened = ImGui::TreeNodeEx((void*)entity->GetID(), flags, name.c_str());

	if (ImGui::IsItemClicked())
	{
		selection = entity;
		inspectorPanel->SetEntity(selection);
		scenePanel->SetEntity(selection);
	}

	if (opened)
	{
		ImGui::TreePop();
	}
}

HierarchyPanel::~HierarchyPanel()
{
	delete inspectorPanel;
}

