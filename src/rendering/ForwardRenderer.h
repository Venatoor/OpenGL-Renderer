#pragma once
#include "Renderer.h"
#include "ShadowMap.h"
#include "ShaderLibrary.h"
#include "Skybox.h"
#include "PostProcesser.h"
class ForwardRenderer : public Renderer
{
public:
	
	ForwardRenderer(int width, int height);
	~ForwardRenderer() = default;
	
	void Render(Scene& scene, Camera& camera, float deltaTime) override;
	PostProcessor& GetPostProcessor() override { return postProcessor; }

	void Initialize(Scene& scene) override;

private:



	void ShadowPass();
	void GeometryPass(Scene& scene, Camera& camera);
	void TransparentPass(Scene& scene, Camera& camera);
	void SkyboxPass(Camera& camera);

	void ApplyCameraUniforms(Shader& shader, Camera& camera);
	void ApplyLightingUniforms(Shader& shader);
	void ApplyCommonUniforms(Shader& shader, bool hasAlpha);

	std::vector<std::pair<float, Entity3D*>> GetSortedTransparentEntities(Scene& scene, Camera& camera);

	ShadowMap shadowMap;
	Skybox skybox;
	PostProcessor postProcessor;
	ShaderLibrary& shaderLib = ShaderLibrary::Get();

	Scene::RenderEntities renderEntities;

	int m_width;
	int m_height;
	const int SHADOW_MAP_TEXTURE_UNIT = 6;

	Shader* currentShader = nullptr;

	void SafeShaderActivate(Shader& shader);

};
