#pragma once
#include <vector>
#include <memory>
#include "Light.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"

class Shader;

class LightManager
{
public:
	void AddDirectionalLight(const DirectionalLight& light);
	void AddPointLight(const PointLight& light);
	void AddSpotLight(const SpotLight& light);

	void ApplyLights(Shader& shader);

	std::vector<DirectionalLight>& GetDirectionalLights() { return directionalLights; }
	std::vector<PointLight>& GetPointLights() { return pointLights;  }
	std::vector<SpotLight>& GetSpotLights() { return spotLights;  }

private:

	std::vector<DirectionalLight> directionalLights;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

};

