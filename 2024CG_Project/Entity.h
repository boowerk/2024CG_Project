#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Entity
{
public:
	Entity();

	virtual std::string GetName() = 0;
	virtual void ShowProperties() = 0;
	virtual glm::mat4 GetModelMat() = 0;
	virtual void UpdateTransform(glm::mat4 newModelMat) = 0;

	uint32_t GetID() { return this->id; }

	virtual ~Entity();

	static uint32_t counter;

protected:
	std::string name;
	glm::mat4	modelMat;

private:
	uint32_t id;
};

