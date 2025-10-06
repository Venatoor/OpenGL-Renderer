#include "PointLight.h"

void PointLight::ApplyToShader(Shader& shader, const std::string& uniformPrefix) const
{
	if (!enabled) return;
	shader.SetVec3(uniformPrefix + ".position", position);
	shader.SetVec3(uniformPrefix + ".color", color);
	shader.SetFloat(uniformPrefix + ".intensity", intensity);
	shader.SetFloat(uniformPrefix + ".radius", radius);
}
