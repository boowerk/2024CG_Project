#include "Animation.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Bone.h"
#include "Model.h"
#include "AssimpGLMHelpers.h"

Animation::Animation(const std::string& animationPath, Model* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);

	if (!scene)
	{
		std::cout << animationPath << " Animation 로드 실패 : " << importer.GetErrorString() << std::endl;
		return;
	}

	assert(scene && scene->mRootNode);
	aiAnimation* animation = scene->mAnimations[0];
	duration = animation->mDuration;
	ticksPerSecond = animation->mTicksPerSecond;
	ReadHierarchyData(rootNode, scene->mRootNode);
	ReadMissingBones(animation, *model);
}

Bone* Animation::FindBone(const std::string& name)
{
	auto iter = std::find_if(bones.begin(), bones.end(),
		[&](const Bone* bone) 
		{
			return bone->GetBoneName() == name;
		}
	);
	if(iter == bones.end()) 
		return nullptr;
	else
		return (*iter);
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
	int size = animation->mNumChannels;

	auto& boneInfoMap = model.GetBoneInfoMap();
	int& boneCount = model.GetBoneCount();

	for (int i = 0; i < size; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;
		//size_t pos = boneName.find('_');
		//if (pos != std::string::npos)
		//	boneName = boneName.substr(pos + 1);

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		this->bones.push_back(new Bone(channel->mNodeName.data, boneInfoMap[boneName].id, channel));
	}

	this->boneInfoMap = boneInfoMap;
}

void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	//std::string nodeName = src->mName.data;
	//size_t pos = nodeName.find('_');
	//if (pos != std::string::npos)
	//	nodeName = nodeName.substr(pos + 1);

	dest.name = src->mName.data;
	dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childcount = src->mNumChildren;

	for (int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}
