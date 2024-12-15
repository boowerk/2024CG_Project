#pragma once

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

class Model;
class Bone;
struct aiAnimation;
struct aiNode;

class Animation
{
public:
	Animation() = default;

	Animation(const std::string& animationPath, Model* model);

	~Animation() = default;

	Bone* FindBone(const std::string& name);

	float GetTicksPerSecond() { return ticksPerSecond; }
	float GetDuration() { return duration; }
	const AssimpNodeData& GetRootNode() { return rootNode; }
	const std::map<std::string, BoneInfo>& GetBoneIDMap() { return boneInfoMap; }

private:
	void ReadMissingBones(const aiAnimation* animation, Model& model);
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

	float duration;
	int ticksPerSecond;
	std::vector<Bone*> bones;
	AssimpNodeData rootNode;
	std::map<std::string, BoneInfo> boneInfoMap;
};

