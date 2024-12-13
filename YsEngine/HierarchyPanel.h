#pragma once

#include <vector>
#include "Panel.h"

class Entity;
class InspectorPanel;
class ScenePanel;

class HierarchyPanel : Panel
{
public:
	HierarchyPanel(std::vector<Entity*>& entityList, ScenePanel* sp);
	virtual void Update() override;
	void UpdateEntityList(std::vector<Entity*>& entityList);
	~HierarchyPanel();

private:
	void DrawEntityNode(Entity* entity);

	std::vector<Entity*>& entityList;
	Entity* selection;

	InspectorPanel* inspectorPanel;
	ScenePanel* scenePanel;
};

