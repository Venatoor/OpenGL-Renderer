#pragma once
#include "Renderer.h"
#include "Skybox.h"
#include "ShaderLibrary.h"
#include "FBO.h"
#include "array"
#include "Texture.h"
#include "BlendManager.h"
#include "GlobalShadingModel.h"

class DeferredRenderer : public Renderer
{
public:

	DeferredRenderer(int width, int height, GlobalShadingModel initialModel);
	~DeferredRenderer() = default;

	void Render(Scene& scene, Camera& camera, float deltaTime) override;
	void Initialize(Scene& scene) override;

	PostProcessor& GetPostProcessor() override { return postProcessor; }

private:

	void GeometryPass(Scene& scene, Camera& camera);
	void LightingPass(Scene& scene, Camera& camera);
	void SkyboxPass(Camera& camera);

	void ApplyLightPassUniforms(Shader& shader);
	void ApplyCameraUniforms(Shader& shader, Camera& camera);

	void CreateGbuffer();
	void SetupLightPassShaders();

	Skybox skybox;
	PostProcessor postProcessor;
	ShaderLibrary& shaderLib = ShaderLibrary::Get();

	Scene::RenderEntities renderEntities;

	int m_width;
	int m_height;

	Shader* currentShader = nullptr;
	Shader* lightPassShader = nullptr;

	void SafeShaderActivate(Shader& shader);

	GLuint quadVAO, quadVBO;
	void InitScreenQuad();

	FBO gBuffer;
	GlobalShadingModel globalShadingModel;

};

