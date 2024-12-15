#pragma once

#include <GL\glew.h>

class Texture
{
public:
	Texture(const char* fileLoc);
	bool LoadTexture(int nChannels = 0, bool preserveData = false);
	void UseTexture(GLenum textureUnit);
	void ClearTexture(); // 메모리에서 텍스쳐 내리기

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	int GetBitDepth() { return bitDepth; }
	unsigned char* GetTexData() { return texData; }

	~Texture();

private:
	GLuint textureID;
	int width, height, bitDepth;

	const char* fileLocation;
	unsigned char* texData;
};

