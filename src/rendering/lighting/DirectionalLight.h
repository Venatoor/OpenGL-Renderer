#pragma once
#include "Light.h"
#include <glm/glm.hpp>

class DirectionalLight : public Light
{
public:
	void ApplyToShader(Shader& shader, const std::string& uniformPrefix) const override;

	glm::vec3 direction = glm::vec3(-0.2f, -0.1f, -0.3f);
};

