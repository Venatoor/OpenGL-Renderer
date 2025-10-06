#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include"shaderClass.h"
#include <map>

class Texture 
{
public:

		enum class Type {

			DIFFUSE = 0,
			SPECULAR,
			EMISSIVE,
			NORMAL,
			HEIGHT,
			ROUGHNESS,
			AO,
			METALLIC,
			OPACITY

		};

	    GLuint ID;
		GLenum glType;
		Type type;
		std::string typeName;
		std::string path;

		Texture() = default;
		Texture(const char* image, Type texType, GLenum glTexType, GLenum slot,GLenum internalFormat, GLenum format, GLenum pixelType, int channelNumber);
		Texture(GLenum texType, GLenum externalformat, GLenum internalformat, GLenum pixelType, int height, int width);
		Texture(GLenum texType, GLenum externalformat, GLenum internalformat, GLenum pixelType, int height, int width, float borderColor[]);

		void TexUnit(Shader& shader, const std::string& uniform, GLuint unit);
	    void Bind() const;
		void Unbind();
		void Delete();

		static std::string TypeToString(Type type) {
			static const std::map<Type, std::string> typeNames = {
				{Type::DIFFUSE, "diffuse"},
				{Type::SPECULAR, "specular"},
				{Type::EMISSIVE, "emissive"},
				{Type::NORMAL, "normal"},
				{Type::HEIGHT, "height"},
				{Type::ROUGHNESS, "roughness"},
				{Type::AO, "ao"},
				{Type::METALLIC, "metallic"},
				{Type::OPACITY, "opacity"}
			};
			return typeNames.at(type);
		}

		int customWidth;
		int customHeight;
};

#endif
