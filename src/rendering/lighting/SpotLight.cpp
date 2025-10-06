#include "SpotLight.h"

void SpotLight::ApplyToShader(Shader& shader, const std::string& uniformPrefix) const
{
	if (!enabled) return;
	shader.SetVec3(uniformPrefix + ".position", position);
	shader.SetVec3(uniformPrefix + ".direction", direction);
	shader.SetVec3(uniformPrefix + ".color", color);
	shader.SetFloat(uniformPrefix + ".intensity", intensity);
	shader.SetFloat(uniformPrefix + ".outerCutOff", outerCutOff);
	shader.SetFloat(uniformPrefix + ".cutOff", cutOff);
	shader.SetFloat(uniformPrefix + ".radius", radius);

}
