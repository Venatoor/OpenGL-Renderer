#pragma once
#include "glad/glad.h"
#include <glm/glm.hpp>

class ComputeImage
{
public:

	ComputeImage(GLsizei width, GLsizei height, GLenum internalFormat, GLenum target = GL_TEXTURE_2D);
	~ComputeImage();

	ComputeImage(const ComputeImage&) = delete;
	ComputeImage& operator=(const ComputeImage&) = delete;

	ComputeImage(ComputeImage&& other) noexcept;
	ComputeImage& operator==(ComputeImage&& other) noexcept;

	void BindImage(GLuint unit, GLenum access) const;
	void UnbindImage(GLuint unit) const;

	void SetData(const void* data, GLenum internalFormat, GLenum type);
	void GetData(void* output, GLenum internalFormat, GLenum type);

	void Clear();

	GLuint GetID() const { return textureID; }
	glm::ivec2 GetSize() const { return {width, height}; }

	void SetBarrier(GLbitfield barrier);


private:

	GLuint textureID;
	GLsizei width;
	GLsizei height;
	GLenum internalFormat;
	GLenum target = GL_TEXTURE_2D;



};

