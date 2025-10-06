#pragma once
#include "IPostProcessEffect.h"

class PostProcessEffect : public IPostProcessEffect
{
public:

	PostProcessEffect(const std::string& name, const std::string& shaderName, bool requiresDepth = false);
	~PostProcessEffect();

	bool RequiresDepthTexture() const override { return requiresDepth; }
	std::string GetName() const override { return effectName; }
	void Apply(GLuint texture, bool lasteffect) override;

	void SetupUniforms();

	Shader& GetShader(const std::string& shaderName);



private:

	bool requiresDepth;
	std::string effectName;
	std::string shaderName;

	Shader& effectShader;

	GLuint quadVAO, quadVBO;

	void RenderQuad();
	void InitRenderData();

};

