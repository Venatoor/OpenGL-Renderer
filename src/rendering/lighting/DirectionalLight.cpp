#include "DirectionalLight.h"

void DirectionalLight::ApplyToShader(Shader& shader, const std::string& uniformPrefix) const
{
	if (!enabled) return;
	shader.SetVec3(uniformPrefix + ".direction", direction);
	shader.SetVec3(uniformPrefix + ".color", color);
	shader.SetFloat(uniformPrefix + ".intensity", intensity);
}
