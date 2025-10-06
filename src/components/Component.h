#pragma once
#include "shaderClass.h"

class Entity3D;

class Component
{
public:

	virtual ~Component() = default;

	Entity3D* owner = nullptr;

	virtual void OnUpdate(float deltaTime) {}
	virtual void OnRender(Shader& shader, const glm::mat4& modelMatrix) {}
};

