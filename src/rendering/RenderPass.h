#pragma once
#include "Scene.h"
#include "Camera.h"

class RenderPass
{
public:

	virtual ~RenderPass();
	virtual void Execute(const Scene& scene, const Camera& camera) = 0;

};

