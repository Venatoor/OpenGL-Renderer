#include "Cubemap.h"
#include<stb/stb_image.h>
#include <iostream>

Cubemap::Cubemap(const std::vector<std::string>& faces) {

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	
	int width, height, channels;

	stbi_set_flip_vertically_on_load(false);
	for (unsigned int i = 0; i < faces.size(); i++ ) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
		if (data) {
			GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format,
				GL_UNSIGNED_BYTE, data);
			std::cout << "Loaded cubemap face " << i << " (" << faces[i]
				<< "): " << width << "x" << height
				<< ", channels: " << channels << std::endl;
			stbi_image_free(data);
		}
		else {
			std::cerr << "Cubemap texture not loaded at path : " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	stbi_set_flip_vertically_on_load(true);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Cubemap::~Cubemap() {

	glDeleteTextures(1, &ID);
}

void Cubemap::Bind(GLenum textureUnit) const {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}