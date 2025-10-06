#include "LightManager.h"
#include "shaderClass.h"

void LightManager::AddDirectionalLight(const DirectionalLight& light)
{
	directionalLights.push_back(light);
}

void LightManager::AddPointLight(const PointLight& light)
{
	pointLights.push_back(light);
}

void LightManager::AddSpotLight(const SpotLight& light)
{
	spotLights.push_back(light);
}

void LightManager::ApplyLights(Shader& shader)
{
	shader.SetInt("numDirLights", (int)directionalLights.size());
	shader.SetInt("numPointLights", (int)pointLights.size());
	shader.SetInt("numSpotLights", (int)spotLights.size());

	for (size_t i = 0; i < directionalLights.size(); i++) {
		directionalLights[i].ApplyToShader(shader, "dirLights[" + std::to_string(i) + "]");
	}
	for (size_t i = 0; i < pointLights.size(); i++) {
		pointLights[i].ApplyToShader(shader, "pointLights[" + std::to_string(i) + "]");
	}

	for (size_t i = 0; i < spotLights.size(); i++) {
		spotLights[i].ApplyToShader(shader, "spotLights[" + std::to_string(i) + "]");
	}
}

