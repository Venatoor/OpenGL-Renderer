#pragma once
#include "LocalShadingModel.h"
#include "Texture.h"
#include <memory>
#include <string>
#include <unordered_map>

enum class BlendMode {

	OPAQUE,
	ALPHA,
	ADDITIVE,
	MULTIPLY
};

class Material {
public:

	explicit Material(Shader& shader,
					  BlendMode blendMode = BlendMode::OPAQUE, 
					  LocalShadingModel localShadingModel = LocalShadingModel::PHONG_BLINN)
		: shader(shader), blendmode(blendMode), localShadingModel(localShadingModel) {

		shader.SetInt("shadingModelID", GetLocalShadingModelID());

	}
	virtual ~Material() = default;

	//virtual void Apply(Shader& shader);
	//void SetShadingModel(std::unique_ptr<IShadingModel> model);

	//Shading Management 

	void SetShader(Shader& newShader) {
		shader = newShader;
	}

	Shader& GetShader() const {
		return shader;
	}

	//Blendmode stuff

	void SetBlendMode(BlendMode mode) {
		blendmode = mode;
	}

	BlendMode GetBlendMode() const {
		return blendmode;
	}

	//ShadingModel stuff

	void SetLocalShadingModel(LocalShadingModel shadingModel) {
		localShadingModel = shadingModel;
		shader.SetInt("shadingModelID", GetLocalShadingModelID());
	}


	LocalShadingModel GetLocalShadingModel() const { return localShadingModel; }
	int GetLocalShadingModelID() { return static_cast<int>(localShadingModel); }

	void AddTexture(std::shared_ptr<Texture> texture) {
		textures.push_back(texture);
	}

	void SetTexture(Texture::Type type, std::shared_ptr<Texture> texture) {
		
		textures.erase(
			std::remove_if(textures.begin(), textures.end(), 
			[type](const std::shared_ptr<Texture>& tex) {
			return tex->type == type;
			}),
			textures.end());

		if (texture) {
			textures.push_back(texture);
		}

	}

	void BindTextures(Shader& shader) const {
		std::unordered_map<Texture::Type, unsigned int> typeCounters;

		for (size_t i = 0; i < textures.size(); i++) {
			const auto& texture = textures[i];
			if (!texture) continue;

			glActiveTexture(GL_TEXTURE0 + i);

			std::string uniformName = Texture::TypeToString(texture->type);

			typeCounters[texture->type]++; 
			uniformName += std::to_string(typeCounters[texture->type]);

			texture->TexUnit(shader, uniformName, i);
			texture->Bind();
		}
	}

	void BindTexture(Shader& shader, Texture::Type type,
		const std::string& uniformName, GLuint unit) const {
		for (const auto& tex : textures) {
			if (tex && tex->type == type) {
				glActiveTexture(GL_TEXTURE0 + unit);
				tex->TexUnit(shader, uniformName, unit);
				tex->Bind();
				return;
			}
		}
	}

protected:

	LocalShadingModel localShadingModel;
	std::vector<std::shared_ptr<Texture>> textures;
	Shader& shader;
	BlendMode blendmode;
};

