#pragma once
#include "Scene.h"
#include "Viewport.h"
#include "shaderClass.h"
#include "Camera.h"
#include "RenderPass.h"
#include "PostProcessingStack.h"
#include "PostProcesser.h"
#include "GlobalShadingModel.h"

class Renderer
{
public:

	virtual ~Renderer() = default;

	virtual void Initialize(Scene& scene) = 0;
	virtual void Render( Scene& scene,  Camera& camera, float deltaTime) = 0;
	virtual PostProcessor& GetPostProcessor() = 0;

	void SetGlobalShadingModel(GlobalShadingModel model) {
		globalShadingModel = model;
	}

	GlobalShadingModel GetGlobalShadingModel() const { return globalShadingModel; }

protected:
	//virtual void OnGlobalShadingModelChange() = 0;

	GlobalShadingModel globalShadingModel = GlobalShadingModel::PHONG_BLINN;

};

