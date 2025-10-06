#pragma once
#include "shaderClass.h"

class IShadingModel
{
public:
	virtual ~IShadingModel() = default;

	virtual void ConfigureShader(Shader& shader) = 0;
	virtual std::string GetShaderDefines() const = 0;

	virtual bool RequiresBlending() const { return false; }
	virtual bool RequiresDepthWrite() const { return true; }
};

