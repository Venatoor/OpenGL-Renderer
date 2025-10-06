#include "ComputeImage.h"
#include <vector>

ComputeImage::ComputeImage(GLsizei width, GLsizei height, GLenum internalFormat, GLenum target) :
	width(width),
	height(height),
	internalFormat(internalFormat),
	target(target)
{
	glGenTextures(1, &textureID);
	glBindTexture(target, textureID);
	glTexStorage2D(target, 1, internalFormat, width, height);

	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

ComputeImage::~ComputeImage()
{
	glDeleteTextures(1, &textureID);
}

ComputeImage::ComputeImage(ComputeImage&& other) noexcept :
	textureID(other.textureID),
	width(other.width),
	height(other.height),
	internalFormat(other.internalFormat),
	target(other.target)
{
	other.width = 0;
	other.height = 0;
	other.textureID = 0;
}

ComputeImage& ComputeImage::operator==(ComputeImage&& other) noexcept
{
	if (this != &other) {
		
		if (textureID) {
			glDeleteTextures(1, &textureID);

		}

		textureID = other.textureID;
		width = other.width;
		height = other.height;
		internalFormat = other.internalFormat;
		target = other.target;

		other.width = 0;
		other.height = 0;
		other.textureID = 0;
	}
	return *this;
}

void ComputeImage::BindImage(GLuint unit, GLenum access) const
{
	glBindImageTexture(unit, textureID, 0, GL_FALSE, 0, access, internalFormat);
}

void ComputeImage::UnbindImage(GLuint unit) const
{
	glBindImageTexture(unit, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);

}

void ComputeImage::SetData(const void* data, GLenum internalFormat, GLenum type)
{
	glBindTexture(target, textureID);
	glTexSubImage2D(target, 0, 0, 0, width, height, internalFormat, type, data);
}

void ComputeImage::GetData(void* output, GLenum internalFormat, GLenum type)
{
	glBindTexture(target, textureID);
	glGetTexImage(target, 0, internalFormat, type, output);
}

void ComputeImage::Clear()
{
	std::vector<uint8_t> zeros(width * height * 4, 0);
	SetData(zeros.data(), GL_RGBA, GL_UNSIGNED_BYTE);
}

void ComputeImage::SetBarrier(GLbitfield barrier)
{
	glMemoryBarrier(barrier);
}
