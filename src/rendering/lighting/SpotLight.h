#pragma once
#include "Light.h"
class SpotLight : public Light
{
public:
	void ApplyToShader(Shader& shader, const std::string& uniformPrefix) const override;

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
	float cutOff = glm::cos(glm::radians(12.5f));
	float outerCutOff = glm::cos(glm::radians(17.5f));
	float radius = 10.0f;
};

