#ifndef COMMONVALS
#define COMMONVALS

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

const int MAX_POINT_LIGHTS = 3;
const int MAX_BONE_COUNT = 120;

struct BoneInfo
{
	int id; // finalBoneMatrices의 index
	glm::mat4 offset;
};

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childcount;
	std::vector<AssimpNodeData> children;
};

#endif