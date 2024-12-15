#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL\glew.h>

#include "CommonValues.h"

class DirectionalLight;
class PointLight;
class Material;

class Shader
{
public:
	Shader();

	void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation,
                        const char* geometryLocation = nullptr, const char* tessControlLocation = nullptr,
                        const char* tessEvalLocation = nullptr);

	std::string ReadFile(const char* fileLocation);

	void UseShader();
	void ClearShader();

	void GetVariableLocations();

	void UseDirectionalLight(DirectionalLight* light);
	void UsePointLights(PointLight** pointLights, unsigned int count);
	void UseMaterial(Material* material);
	void UseFinalBoneMatrices(const std::vector<glm::mat4>& transforms);
    void UseEyePos(glm::vec3 pos);

	GLuint GetID() { return shaderID; }

	GLuint GetModelMatLoc() { return modelMatLoc; }
	GLuint GetPVMLoc() { return PVMLoc; }
	GLuint GetColorSamplerLoc() { return colorSamplerLoc; }
	GLuint GetNormalSamplerLoc() { return normalSamplerLoc; }
	GLuint GetNormalMatLoc() { return normalMatLoc; }
	GLuint GetEyePosLoc() { return eyePosLoc; }
	GLuint GetFinalBonesMatricesLoc() { return finalBonesMatricesLoc; }

    // utility uniform functions
// ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(shaderID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(shaderID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(shaderID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(shaderID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(shaderID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

	~Shader();

private:
	// handles
	GLuint shaderID,
		modelMatLoc, PVMLoc, 
		colorSamplerLoc, normalSamplerLoc,
		normalMatLoc,
		eyePosLoc, finalBonesMatricesLoc,
		pointLightCountLoc;

	struct
	{
		GLuint ambientIntensityLoc;
		GLuint diffuseIntensityLoc;
		GLuint colorLoc;

		GLuint directionLoc;
	} directionalLightLoc;

	struct
	{
		GLuint ambientIntensityLoc;
		GLuint diffuseIntensityLoc;
		GLuint colorLoc;

		GLuint positionLoc;
		GLuint constantLoc;
		GLuint linearLoc;
		GLuint exponentLoc;
	} pointLightLoc[MAX_POINT_LIGHTS];

	struct
	{
		GLuint specularIntensityLoc;
		GLuint shininessLoc;
	} materialLoc;

	void CompileShader(const char* vertexCode, const char* fragmentCode, 
                    const char* geometryCode = nullptr, const char* tessControlCode = nullptr,
                    const char* tessEvalCode = nullptr);
	void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};

