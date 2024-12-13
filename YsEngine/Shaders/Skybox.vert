#version 460

layout (location = 0) in vec3 pos;

out vec3 TexCoord;

uniform mat4 PVM;

void main()
{
	TexCoord = pos;
	gl_Position = PVM * vec4(pos, 1.f);
}