#include "Shader.h"

#include "DirectionalLight.h"
#include "Material.h"
#include "PointLight.h"

Shader::Shader()
{
	shaderID = 0;

	modelMatLoc=0;
	PVMLoc = 0;
	colorSamplerLoc = 0;
	normalSamplerLoc = 0;
	normalMatLoc = 0;
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation,
							const char* geometryLocation, const char* tessControlLocation,
							const char* tessEvalLocation)
{	
	std::string vertexStr = ReadFile(vertexLocation).c_str();
	const char* vertexCode = vertexStr.c_str();

	std::string fragmentStr = ReadFile(fragmentLocation).c_str();
	const char* fragmentCode = fragmentStr.c_str();

	const char* geometryCode = nullptr;
	std::string geometryStr = "";
	if (geometryLocation)
	{
		geometryStr = ReadFile(geometryLocation).c_str();
		geometryCode = geometryStr.c_str();
	}
		
	const char* tessControlCode = nullptr;
	std::string tessControlStr = "";
	if (tessControlLocation)
	{
		tessControlStr = ReadFile(tessControlLocation).c_str();
		tessControlCode = tessControlStr.c_str();
	}

	const char* tessEvalCode = nullptr;
	std::string tessEvalStr = "";
	if (tessEvalLocation)
	{
		tessEvalStr = ReadFile(tessEvalLocation).c_str();
		tessEvalCode = tessEvalStr.c_str();
	}

	CompileShader(vertexCode, fragmentCode, geometryCode ,tessControlCode, tessEvalCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open())
	{
		printf("Failed to read %s! File doesn't exist.", fileLocation);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode,
						const char* geometryCode, const char* tessControlCode,
						const char* tessEvalCode)
{
	shaderID = glCreateProgram();

	if (!shaderID)
	{
		printf("Error creating shader program!\n");
		assert(0);
		return;
	}

	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);
	if (geometryCode)
		AddShader(shaderID, geometryCode, GL_GEOMETRY_SHADER);
	if (tessControlCode)
		AddShader(shaderID, tessControlCode, GL_TESS_CONTROL_SHADER);
	if (tessEvalCode)
		AddShader(shaderID, tessEvalCode, GL_TESS_EVALUATION_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	// 쉐이더 컴파일 성공, 쉐이더 내부 변수들 위치 가져오기
	GetVariableLocations();
}

void Shader::UseShader()
{
	glUseProgram(shaderID);
}

void Shader::ClearShader()
{
	if (shaderID != 0)
	{
		glDeleteProgram(shaderID);
		shaderID = 0;
	}
}

void Shader::GetVariableLocations()
{
	modelMatLoc = glGetUniformLocation(shaderID, "modelMat");
	PVMLoc = glGetUniformLocation(shaderID, "PVM");
	colorSamplerLoc = glGetUniformLocation(shaderID, "colorSampler");
	normalSamplerLoc = glGetUniformLocation(shaderID, "normalSampler");
	normalMatLoc = glGetUniformLocation(shaderID, "normalMat");

	directionalLightLoc.ambientIntensityLoc = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
	directionalLightLoc.diffuseIntensityLoc = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");
	directionalLightLoc.colorLoc = glGetUniformLocation(shaderID, "directionalLight.base.color");
	directionalLightLoc.directionLoc = glGetUniformLocation(shaderID, "directionalLight.direction");

	materialLoc.specularIntensityLoc = glGetUniformLocation(shaderID, "material.specularIntensity");
	materialLoc.shininessLoc = glGetUniformLocation(shaderID, "material.shininess");

	eyePosLoc = glGetUniformLocation(shaderID, "eyePosition");
	finalBonesMatricesLoc = glGetUniformLocation(shaderID, "finalBonesMatrices");

	for (int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		char locbuff[100] = {'\0'};

		snprintf(locbuff, sizeof(locbuff), "pointLights[%d].base.ambientIntensity", i);
		pointLightLoc[i].ambientIntensityLoc = glGetUniformLocation(shaderID, locbuff);

		snprintf(locbuff, sizeof(locbuff), "pointLights[%d].base.diffuseIntensity", i);
		pointLightLoc[i].diffuseIntensityLoc = glGetUniformLocation(shaderID, locbuff);

		snprintf(locbuff, sizeof(locbuff), "pointLights[%d].base.color", i);
		pointLightLoc[i].colorLoc = glGetUniformLocation(shaderID, locbuff);

		snprintf(locbuff, sizeof(locbuff), "pointLights[%d].position", i);
		pointLightLoc[i].positionLoc = glGetUniformLocation(shaderID, locbuff);

		snprintf(locbuff, sizeof(locbuff), "pointLights[%d].constant", i);
		pointLightLoc[i].constantLoc = glGetUniformLocation(shaderID, locbuff);

		snprintf(locbuff, sizeof(locbuff), "pointLights[%d].linear", i);
		pointLightLoc[i].linearLoc = glGetUniformLocation(shaderID, locbuff);

		snprintf(locbuff, sizeof(locbuff), "pointLights[%d].exponent", i);
		pointLightLoc[i].exponentLoc = glGetUniformLocation(shaderID, locbuff);
	}
	pointLightCountLoc = glGetUniformLocation(shaderID, "pointLightCount");
}

void Shader::UseDirectionalLight(DirectionalLight* light)
{
	glUniform1f(directionalLightLoc.ambientIntensityLoc, light->ambientIntensity);
	glUniform1f(directionalLightLoc.diffuseIntensityLoc, light->diffuseIntensity);
	glUniform4f(directionalLightLoc.colorLoc, 
		light->color.r,
		light->color.g,
		light->color.b,
		light->color.a);
	glUniform3f(directionalLightLoc.directionLoc, 
		light->direction.x,
		light->direction.y,
		light->direction.z);
}

void Shader::UsePointLights(PointLight** pointLights, unsigned int count)
{
	glUniform1i(pointLightCountLoc, count);
	for (int i = 0; i < count; i++)
	{
		glUniform1f(pointLightLoc[i].ambientIntensityLoc, pointLights[i]->ambientIntensity);
		glUniform1f(pointLightLoc[i].diffuseIntensityLoc, pointLights[i]->diffuseIntensity);
		glUniform4f(pointLightLoc[i].colorLoc, 
			pointLights[i]->color.r,
			pointLights[i]->color.g, 
			pointLights[i]->color.b, 
			pointLights[i]->color.a);
		glUniform3f(pointLightLoc[i].positionLoc,
			pointLights[i]->position.x,
			pointLights[i]->position.y, 
			pointLights[i]->position.z);
		glUniform1f(pointLightLoc[i].constantLoc, pointLights[i]->constant);
		glUniform1f(pointLightLoc[i].linearLoc, pointLights[i]->linear);
		glUniform1f(pointLightLoc[i].exponentLoc, pointLights[i]->exponent);
	}
}

void Shader::UseMaterial(Material* material)
{
	glUniform1f(materialLoc.specularIntensityLoc, material->specular);
	glUniform1f(materialLoc.shininessLoc, material->shininess);
}

void Shader::UseFinalBoneMatrices(const std::vector<glm::mat4>& transforms)
{
	for (int i = 0; i < transforms.size(); i++)
		glUniformMatrix4fv(finalBonesMatricesLoc + i, 1, false, glm::value_ptr(transforms[i]));
}

void Shader::UseEyePos(glm::vec3 pos)
{
	glUniform3f(eyePosLoc, pos.x, pos.y, pos.z);
}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

Shader::~Shader()
{
	ClearShader();
}
