#include "MSAAFBO.h"

MSAAFBO::MSAAFBO(int width, int height, bool createDepthStencil, unsigned int samples) :
	width(width), height(height), samples(samples)
{
	glGenFramebuffers(1, &msaaFBO);
	Bind();

	CreateMultisampleColorBuffer();

	if (createDepthStencil) {

		CreateDepthStencilBuffer();
	}

	CheckStatus();
	
	resolvedTexture = Texture(GL_TEXTURE_2D,GL_RGB, GL_RGB16F, GL_UNSIGNED_BYTE, height, width);
	Unbind();
}

MSAAFBO::~MSAAFBO()
{
	if (hasDepthStencil) glDeleteRenderbuffers(1, &rbo);
	glDeleteTextures(1, &msaaColorBuffer);
	glDeleteFramebuffers(1, &msaaFBO);
}

void MSAAFBO::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, msaaFBO);
}

void MSAAFBO::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool MSAAFBO::CheckStatus() const
{
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: MSAA Framebuffer is not complete!" << std::endl;
		throw std::runtime_error("Framebuffer not complete!");
	}
}

void MSAAFBO::Resize(int newWidth, int newHeight)
{
	//L3GZ 3WTANI HHHHHHHHHHHHHHHHHHH
}

void MSAAFBO::ResolveToTexture(GLuint targetFBO)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFBO);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO); 

	glBlitFramebuffer(0, 0, width, height,
		0, 0, width, height,
		GL_COLOR_BUFFER_BIT, GL_LINEAR);

}

void MSAAFBO::CreateMultisampleColorBuffer()
{
	glGenTextures(1, &msaaColorBuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaaColorBuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaaColorBuffer, 0);
}

void MSAAFBO::CreateDepthStencilBuffer()
{
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	hasDepthStencil = true;
}
