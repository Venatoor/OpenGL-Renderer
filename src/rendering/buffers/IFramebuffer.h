#pragma once
#include <glad/glad.h>
#include "Texture.h"
#include <optional>

enum class FramebufferType {
	STENCIL,
	DEPTH,
	DEPTH_STENCIL
};

class IFramebuffer
{
public:
	virtual ~IFramebuffer() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;

	virtual bool CheckStatus() const = 0;

	virtual void Resize(int newWidth, int newHeight) = 0;


};

