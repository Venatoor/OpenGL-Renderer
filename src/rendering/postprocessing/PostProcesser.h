#pragma once 
#include <glad/glad.h>
#include"shaderClass.h"
#include "FramebufferFactory.h"
#include"Texture.h"
#include"FBO.h"


class PostProcessor {

private:

	Shader& postProcessingShader;
	std::unique_ptr<IFramebuffer> msaaFBO;
	std::unique_ptr<FBO> resolvedFBO;
	uint32_t activeEffects = 0;
	unsigned int msaaSamples;

	void InitRenderData();
	GLuint quadVAO, quadVBO;

public:

	PostProcessor(int width, int height, Shader& shader, unsigned int msaaSamples = 1 );
	~PostProcessor();

	enum Effect : uint32_t {
		NONE = 0,
		INVERT = 1 << 0,
		GRAYSCALE = 1 << 1,
		EDGE_DETECT = 1 << 2,
		CHROMATIC_AB = 1 << 3,
		BLUR = 1 << 4
	};

	void BeginRender();
	void EndRender();
	void Render(float time);

	void EnableEffect(uint32_t effects) { activeEffects = effects; }
	void ToggleEffect(Effect effect) { activeEffects ^= effect; }
	bool isEffectEnabled(Effect effect) const { return (activeEffects & effect) != 0; }
	
	void SetMSAASamples(unsigned int samples);


};