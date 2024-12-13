#pragma once

#include <vector>

#include "GL/glew.h"
#include "Entity.h"
#include "Vertex.h"

class Shader;
class Texture;

class Terrain : public Entity
{
public:	
	Terrain();

	void LoadTerrain(const char* fileLoc);
	void DrawTerrain(glm::mat4 viewMat, glm::mat4 projMat);
	Shader* GetShader() { return terrainShader; }
	void UseShader();

	virtual std::string GetName() override;
	virtual void ShowProperties() override;
	virtual glm::mat4 GetModelMat() override;
	virtual void UpdateTransform(glm::mat4 newModelMat) override;

	GLfloat GetHeight(float worldX, float worldZ);
	~Terrain();

private:
	Shader* terrainShader;
	GLuint loc_PVM;

	int width, height, nChannels;

	std::vector<Vertex> vertices;

	GLuint VAO, VBO;
	unsigned int rez;

	Texture* heightMap;
	Texture* diffuseMap;

	float heightScale;
	float heightShift;

	glm::vec3 position;
	std::vector<std::vector<GLfloat>> heights;
};

