#version 460

in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

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

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform sampler2D colorSampler;
uniform sampler2D normalSampler;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;
uniform vec3 eyePosition;
uniform Material material;

vec4 CalcLightByDirection(Light light, vec3 direction, vec3 normal)
{
	direction = normalize(direction);
	normal = normalize(normal);

	vec4 ambientColor = (light.ambientIntensity * light.color);
	
	float diffuseFactor = max(dot(normal, direction), 0);
	vec4 diffuseColor = (light.diffuseIntensity * light.color) * diffuseFactor;

	vec3 reflectVec = normalize(reflect(-direction, normal));
	vec3 fragToEye = normalize(eyePosition - FragPos);

	float specularFactor = max(dot(reflectVec, fragToEye), 0);
	vec4 specularColor = (material.specularIntensity * light.color) 
		* pow(specularFactor, material.shininess);

	return ambientColor + diffuseColor + specularColor;
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
	vec4 texColor = texture(colorSampler, TexCoord);
	vec3 texNormal = texture(normalSampler, TexCoord).rgb; // normal map에서 떼어오기
	texNormal = texNormal * 2.0 - 1.0; // 값의 범위를 [0,1]에서 [-1,1]로 remap
	texNormal = normalize(TBN * texNormal);
	
	vec4 finalColor = vec4(0,0,0,0);
	finalColor += CalcDirectionalLight(texNormal);
	finalColor += CalcPointLights(texNormal);	

	FragColor = texColor * finalColor;
}