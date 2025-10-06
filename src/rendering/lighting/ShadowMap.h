#pragma once
#include "SMFBO.h"
#include "DirectionalLight.h"  // Include your light class
#include <glm/glm.hpp>
#include "IFramebuffer.h"
#include "SpotLight.h"


class ShadowMap
{
public:
	ShadowMap(int width = 1024, int height = 1024);
	~ShadowMap();

	void UpdateFromDirectionalLight(const DirectionalLight& light,
		const glm::vec3& focusPoint = glm::vec3(0.0f),
		float coverage = 20.f);

	void UpdateFromSpotLight(const SpotLight& light);

	void BeginShadowPass();
	void EndShadowPass();
	void BindTexture(GLenum textureUnit) const;

	void SetLightViewProj(const glm::mat4& viewProj) { lightViewProj = viewProj; }
	const glm::mat4& GetLightViewProj() const { return lightViewProj; }

private:

	std::unique_ptr<IFramebuffer> framebuffer;
	glm::mat4 lightViewProj;
	int width;
	int height;

	SMFBO* GetSMFBO() const;


};

