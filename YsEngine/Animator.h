#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

class Animation;

class Animator
{
public:
	Animator(Animation* animation);

	void UpdateAnimation(float deltaTime);
	void PlayAnimation(Animation* pAnimation);
	void CalculateBoneTransform (const AssimpNodeData* node, glm::mat4 parentTransform);
	std::vector<glm::mat4> GetFinalBoneMatrices();
	Animation* GetCurrAnimation() { return currentAnimation; }
	void Reset();

private:
	std::vector<glm::mat4> finalBoneMatrices;
	Animation* currentAnimation;
	float currentTime;
	float deltaTime;
};

