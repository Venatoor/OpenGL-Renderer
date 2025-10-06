#include"Texture.h"
#include"array"


Texture::Texture(const char* image, Type texType, GLenum glTexType, GLenum slot, GLenum internalFormat, GLenum format, GLenum pixelType, int channelNumber) {

	glType = glTexType;
	type = texType;
	customHeight = 0;
	customWidth = 0;

	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, channelNumber);

	glGenTextures(1, &ID);
	glActiveTexture(slot); //In our case GL_TEXTURE0
	glBindTexture(glTexType, ID);// In our case GL_TEXTURE_2D

	glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(glTexType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(glTexType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(glTexType, 0, internalFormat, widthImg, heightImg, 0, format, pixelType, bytes); // in our case format is GL_RGBA, pixelType is G
	glGenerateMipmap(glTexType);

	stbi_image_free(bytes);
	glBindTexture(glTexType, 0);
}

Texture::Texture(GLenum texType, GLenum externalformat, GLenum internalformat, GLenum pixelType, int height, int width)
{
	glType = texType;
	typeName = "";

	customHeight = height;
	customWidth = width;
	

	glGenTextures(1, &ID);
	glBindTexture(texType, ID);
	glTexImage2D(texType, 0, internalformat, customWidth, customHeight, 0, externalformat, pixelType, NULL);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(texType, 0);

}

Texture::Texture(GLenum texType, GLenum externalformat, GLenum internalformat, GLenum pixelType, int height, int width, float borderColor[]) {

	glType = texType;
	typeName = "";

	customHeight = height;
	customWidth = width;

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, externalformat, pixelType, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);




}

void Texture::TexUnit(Shader& shader, const std::string& uniform, GLuint unit) {

	GLuint texUni = glGetUniformLocation(shader.ID, uniform.c_str());
	shader.Activate();
	glUniform1i(texUni, unit);
}

void Texture::Bind() const
{
	if (ID == 0) {
		std::cerr << "ERROR: Trying to bind invalid texture (ID=0)" << std::endl;
		return;
	}
	glBindTexture(glType, ID);
}

void Texture::Unbind()
{
	glBindTexture(glType, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}
