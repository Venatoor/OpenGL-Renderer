#pragma once
#include "IFramebuffer.h"
#include "Texture.h"
#include "array"

class SMFBO : public IFramebuffer
{	
public:

	SMFBO(int width, int height);
	~SMFBO();

	void Bind() const override;
	void Unbind() const override;

	GLuint GetDepthTexture() const { return depthTexture.ID; }
	void BindDepthTexture() const;

	void SetBorderColor(float r, float g, float b, float a);

	const Texture& GetColorAttachment() const;

	bool CheckStatus() const override;
	void Resize(int newWidth, int newHeight) override;

	int GetWidth() const override { return width; }
	int GetHeight() const override { return height; }

	GLuint GetID() { return fboID; }

private:
	int width;
	int height;
	GLuint fboID;
	Texture depthTexture;

	float borderColor[4];
};

