#pragma once
#include <glm/glm.hpp>
#include "shaderClass.h"
#include "Component.h"

class Light : public Component
{
public:
	virtual ~Light() = default;
	virtual void ApplyToShader(Shader& shader, const std::string& uniformPrefix) const = 0;
	
	glm::vec3 color = glm::vec3(1.0f);
	float intensity = 1.0f;
	//TODO : LIGHT IS A TYPE OF GAMEOBJECT, TO EXPAND LATER ON 
	bool enabled = true;
};

