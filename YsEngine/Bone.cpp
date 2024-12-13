#include "Bone.h"

#include <iostream>

#include "AssimpGLMHelpers.h"

Bone::Bone(const std::string& name, int id, const aiNodeAnim* channel) :
	name(name), id(id),	localTransform(1.f)
{
	numPositions = channel->mNumPositionKeys;
	for (int positionIndex = 0; positionIndex < numPositions; positionIndex++)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;
		data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
		data.timeStamp = timeStamp;
		positions.push_back(data);
	}

	numRotations = channel->mNumRotationKeys;
	for (int rotationIndex = 0; rotationIndex < numRotations; rotationIndex++)
	{
		aiQuaternion aiRotation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation data;
		data.orientation = AssimpGLMHelpers::GetGLMQuat(aiRotation);
		data.timeStamp = timeStamp;
		rotations.push_back(data);
	}

	numScales = channel->mNumScalingKeys;
	for (int scaleIndex = 0; scaleIndex < numScales; scaleIndex++)
	{
		aiVector3D aiScale = channel->mScalingKeys[scaleIndex].mValue;
		float timeStamp = channel->mScalingKeys[scaleIndex].mTime;
		KeyScale data;
		data.scale = AssimpGLMHelpers::GetGLMVec(aiScale);
		data.timeStamp = timeStamp;
		scales.push_back(data);
	}
}

void Bone::Update(float animationTime)
{
	glm::mat4 T = InterpolatePosition(animationTime);
	glm::mat4 R = InterpolateRotation(animationTime);
	glm::mat4 S = InterpolateScale(animationTime);
	localTransform = T*R*S;
}

int Bone::GetPositionIndex(float animationTime)
{
	for (int i = 0; i < numPositions - 1; i++)
	{
		if (animationTime < positions[i + 1].timeStamp)
			return i;
	}
	assert(0);
}

inline int Bone::GetRotationIndex(float animationTime)
{
	for (int i = 0; i < numRotations - 1; i++)
	{
		if (animationTime < rotations[i + 1].timeStamp)
			return i;
	}
	assert(0);
}

int Bone::GetScaleIndex(float animationTime)
{
	for (int i = 0; i < numScales - 1; i++)
	{
		if (animationTime < scales[i + 1].timeStamp)
			return i;
	}
	assert(0);
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float scaleFactor = 0.f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
	if(numPositions == 1)
		return glm::translate(glm::mat4(1.f), positions[0].position);

	int p0Index = GetPositionIndex(animationTime);
	int p1Index = p0Index+1;
	float scaleFactor = GetScaleFactor(positions[p0Index].timeStamp,
		positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(positions[p0Index].position, 
		positions[p1Index].position, scaleFactor);
	return glm::translate(glm::mat4(1.f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
	if (numRotations == 1)
	{
		auto rotation = glm::normalize(rotations[0].orientation);
		return glm::mat4_cast(rotation);
	}
		

	int p0Index = GetRotationIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(rotations[p0Index].timeStamp,
		rotations[p1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(rotations[p0Index].orientation,
		rotations[p1Index].orientation, scaleFactor);
	return glm::mat4_cast(finalRotation);
}

glm::mat4 Bone::InterpolateScale(float animationTime)
{
	if (numScales == 1)
		return glm::scale(glm::mat4(1.f), scales[0].scale);

	int p0Index = GetScaleIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(scales[p0Index].timeStamp,
		scales[p1Index].timeStamp, animationTime);
	glm::vec3 finalScale = glm::mix(scales[p0Index].scale,
		scales[p1Index].scale, scaleFactor);
	return glm::scale(glm::mat4(1.f), finalScale);
}
