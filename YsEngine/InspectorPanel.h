#pragma once
#include "Panel.h"

class Model;
class DirectionalLight;
class Entity;

class InspectorPanel: public Panel
{
public:
	InspectorPanel() : currEntity(nullptr) {}

	virtual void Update() override;

	void SetEntity(Entity* e);

	~InspectorPanel();

private:
	Entity* currEntity;
};

