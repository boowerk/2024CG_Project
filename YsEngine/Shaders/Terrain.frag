#version 460 core

in float Height;
in vec2 TexCoord_global;
in vec2 TexCoord_local;
in vec3 FragPos;

out vec4 FragColor;

const int MAX_POINT_LIGHTS = 3;

struct Light
{
	vec4 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

uniform sampler2D diffuseSampler;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

uniform vec2 texelSize;
uniform sampler2D heightSampler;
uniform float HEIGHT_SCALE;

vec4 CalcLightByDirection(Light light, vec3 direction, vec3 normal)
{
	direction = normalize(direction);
	normal = normalize(normal);

	vec4 ambientColor = (light.ambientIntensity * light.color);
	
	float diffuseFactor = max(dot(normal, direction), 0);
	vec4 diffuseColor = (light.diffuseIntensity * light.color) * diffuseFactor;

	return ambientColor + diffuseColor;
}

vec4 CalcDirectionalLight(vec3 normal)
{
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, normal);
}

vec4 CalcPointLights(vec3 normal)
{
	vec4 totalColor = vec4(0,0,0,0);
	for(int i = 0; i < pointLightCount; i++)
	{
		vec3 direction = pointLights[i].position - FragPos;
		vec4 color = CalcLightByDirection(pointLights[i].base, direction, normal);

		float distance = length(pointLights[i].position - FragPos);

		float attenuation =	
			pointLights[i].exponent * distance * distance + 
			pointLights[i].linear * distance + 
			pointLights[i].constant;

		totalColor += (color / attenuation);
	}
	return totalColor;
}

void main()
{
    vec4 texColor = texture(diffuseSampler, TexCoord_local);

	// 인접한 텍셀 4개를 쭉 긁어와서 normal 계산
	float left  = texture(heightSampler, TexCoord_global + vec2(-texelSize.x, 0.0)).r * HEIGHT_SCALE * 2.0 - 1.0;
	float right = texture(heightSampler, TexCoord_global + vec2( texelSize.x, 0.0)).r * HEIGHT_SCALE * 2.0 - 1.0;
	float up    = texture(heightSampler, TexCoord_global + vec2(0.0,  texelSize.y)).r * HEIGHT_SCALE * 2.0 - 1.0;
	float down  = texture(heightSampler, TexCoord_global + vec2(0.0, -texelSize.y)).r * HEIGHT_SCALE * 2.0 - 1.0;
	vec3 normal = normalize(vec3(down - up, 2.0, left - right));

	vec4 finalColor = vec4(0,0,0,0);
	finalColor += CalcDirectionalLight(normal);
	finalColor += CalcPointLights(normal);	

	FragColor = texColor * finalColor;
	FragColor +=  Height/128.f;
}