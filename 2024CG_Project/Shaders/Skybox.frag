#version 460

in vec3 TexCoord;

out vec4 FragColor;

uniform samplerCube sampler;

void main()
{	
	FragColor = texture(sampler, TexCoord);
}