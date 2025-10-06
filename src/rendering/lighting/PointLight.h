#pragma once
#include "Light.h"
#include "glm/glm.hpp"

class PointLight : public Light
{
public:
	void ApplyToShader(Shader& shader, const std::string& uniformPrefix) const override;

	glm::vec3 position = glm::vec3(0.0f);
	float radius = 10.0f;
};

