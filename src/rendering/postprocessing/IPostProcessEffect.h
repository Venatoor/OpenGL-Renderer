#pragma once
#include "IFramebuffer.h"
class IPostProcessEffect
{
public:
	virtual ~IPostProcessEffect() = default;


	virtual void Apply(
		GLuint inputTexture,
		bool lastEffect) = 0;
	virtual std::string GetName() const = 0;

	virtual bool RequiresDepthTexture() const { return false; }
};

