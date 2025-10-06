#pragma once

#include <vector>
#include <string>
#include <glad/glad.h>
#include "Cubemap.h"

class Cubemap
{
public:

	Cubemap(const std::vector<std::string>& faces);
	~Cubemap();

	void Bind(GLenum textureUnit = GL_TEXTURE0) const;
	GLuint GetID() const { return ID; }

private:
	GLuint ID;

};

