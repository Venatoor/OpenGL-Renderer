#include "Texture3D.h"
#include <stdexcept>

Texture3D::Texture3D(GLenum internalFormat, GLenum format, GLenum type, int width, int height, int depth,
	std::optional<const void*> data,GLenum slot, bool generateMipmaps)
	: width(width),
	  height(height),
	  depth(depth),
	  hasMipMaps(generateMipmaps),
	  slot(slot)
{ 
	if (width <= 0 || height <= 0 || depth <= 0) {
		throw std::invalid_argument("Texture dimensions must be positive and superior to 0 !");
	}

	glGenTextures(1, &id);
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_3D, id);

	GLsizei levels = 1;
	if (hasMipMaps) {
		levels = 1 + floor(log2(std::max(width, std::max(height, depth))));
	}
		
	glTexStorage3D(GL_TEXTURE_3D, levels, internalFormat, width, height, depth);

	if (data.has_value()) {
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width, height, depth, format, type, data.value());
	}

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, hasMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, hasMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);


	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	if (hasMipMaps) {
		glGenerateMipmap(GL_TEXTURE_3D);
	}
	glBindTexture(GL_TEXTURE_3D, 0);
}


Texture3D::Texture3D(Texture3D&& other) noexcept :
	id(other.id),
	height(other.height),
	depth(other.depth),
	width(other.width),
	slot(other.slot)
{
	other.id = 0;
}

Texture3D& Texture3D::operator=(Texture3D&& other) noexcept
{
	if (this != &other) {
		if (id) {
			glDeleteTextures(1, &id);
		}

		id = other.id;
		width = other.width;
		height = other.height;
		depth = other.depth;

		other.id = 0;
	}
	return *this;
}

Texture3D::~Texture3D()
{
	if (id) {
		glDeleteTextures(1, &id);
	}
}

void Texture3D::Bind() const
{
	glBindTexture(GL_TEXTURE_3D, id);
}

void Texture3D::Unbind()
{
	glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3D::UploadData(const void* data, GLenum format, GLenum type,
	int xOffset, int yOffset, int zOffset, int subWidth, int subHeight, int subDepth)
{
	if (!data) return;

	subWidth = (subWidth < 0) ? width : subWidth;
	subHeight = (subHeight < 0) ? height : subHeight;
	subDepth = (subDepth < 0) ? depth : subDepth;

	glBindTexture(GL_TEXTURE_3D, id);
	glTexSubImage3D(GL_TEXTURE_3D, 0,
		xOffset, yOffset, zOffset,
		subWidth, subHeight, subDepth,
		format, type, data);
	glGenerateMipmap(GL_TEXTURE_3D);

	glBindTexture(GL_TEXTURE_3D, 0);
}

