#pragma once
#include <glad/glad.h>
#include <iostream>
#include "IFramebuffer.h"
#include <optional>
#include <vector>
#include "Texture.h"
#include <array>

class FBO : public IFramebuffer 
{

public:

	static constexpr size_t MAX_ATTACHEMENTS = 8;

private:
	GLuint ID;
	int width;
	int height;

	std::optional<Texture> colorAttachment;

	
	//std::vector<Texture> colorAttachments;

	std::array<Texture, MAX_ATTACHEMENTS> colorAttachments;
	std::array<GLenum, MAX_ATTACHEMENTS> drawBuffers;

	std::optional<Texture> depthTexture;
	GLuint rbo = 0;
	FramebufferType depthType;
	bool hasDepthStencil = false;
	bool createDepthStencil = true;
	bool depthAsTexture = false;

public:
	
	FBO() = default;
	
	FBO(int width, int height, FramebufferType depthStencilType,
		bool useMultipleColorAttachments, std::optional<std::array<Texture, MAX_ATTACHEMENTS>>&& textures
		,bool createDepthStencil, bool depthAsTexture,
		size_t numColorAttachments);
	~FBO();

	void Bind() const;
	void Unbind() const;

	void BlitToFBO(GLuint targetFBO, GLbitfield mask) const;
	void BlitToDefaultFramebuffer(int windowWidth, int windowHeight, GLbitfield mask) const;

	const Texture& GetColorAttachment(size_t index = 0) const;
	const std::optional<Texture>& GetDepthTexture() const { return depthTexture; }
	const std::array<Texture, MAX_ATTACHEMENTS>& GetAllColorAttachments() const { return colorAttachments; }
	size_t GetColorAttachmentCount() const;

	bool HasDepthStencil() const  { return hasDepthStencil; }
	FramebufferType GetDepthStencilType() { return depthType;  }

	//Framebuffer Operations
	bool CheckStatus() const override;
	void Resize(int newWidth, int newHeight) override;

	int GetWidth() const override { return width; }
	int GetHeight() const override { return height;  }

	GLuint GetID() { return ID; }

private:

	void InitializeColorAttachments(bool useMultiple, size_t count);
	void AttachDepthStencil(FramebufferType type);

};

