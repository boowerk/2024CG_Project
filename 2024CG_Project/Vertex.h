#pragma once

#include "glm/glm.hpp"

const int MAX_BONE_INFLUENCE = 4;

struct Vertex
{
	Vertex() {}

	Vertex(float px, float py, float pz)
	{
		Position.x = px; Position.y = py; Position.z = pz;
		TexCoords = glm::vec2(0.f, 0.f);
		Normal = glm::vec3(0.f, 0.f, 0.f);
		Tangent = glm::vec3(0.f, 0.f, 0.f);
		Bitangent = glm::vec3(0.f, 0.f, 0.f);
	}

	glm::vec3 Position;
	glm::vec2 TexCoords;
	glm::vec3 Normal;

	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;

	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};