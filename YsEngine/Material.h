#pragma once

#include <GL\glew.h>

class Material
{
public:
	Material(GLfloat specular, GLfloat shininess) : specular(specular), shininess(shininess) {}

	GLfloat specular;
	GLfloat shininess;
};

