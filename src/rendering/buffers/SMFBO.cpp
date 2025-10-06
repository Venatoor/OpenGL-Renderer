#include "SMFBO.h"


SMFBO::SMFBO(int width, int height)
	: width(width), height(height)
{
	borderColor[0] = 1.0f;
	borderColor[1] = 1.0f;
	borderColor[2] = 1.0f;
	borderColor[3] = 1.0f;

	depthTexture = Texture(GL_TEXTURE_2D, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT, height, width, borderColor);
	
	glGenFramebuffers(1, &fboID);

	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture.ID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	CheckStatus();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

SMFBO::~SMFBO()
{
	if (fboID) {
		glDeleteFramebuffers(1, &fboID);
	}
}

void SMFBO::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
}

void SMFBO::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SMFBO::BindDepthTexture() const
{
	glBindTexture(GL_TEXTURE_2D, depthTexture.ID);
}

void SMFBO::SetBorderColor(float r, float g, float b, float a)
{
	depthTexture.Bind();
	float borderColor[] = { r, g, b, a };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

const Texture& SMFBO::GetColorAttachment() const
{
	// this is my oopsy my interface is not well defined 
	return depthTexture;
}

bool SMFBO::CheckStatus() const
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		const char* error = "";
		switch (status) {
		case GL_FRAMEBUFFER_UNDEFINED: error = "UNDEFINED"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: error = "INCOMPLETE_ATTACHMENT"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: error = "MISSING_ATTACHMENT"; break;
		case GL_FRAMEBUFFER_UNSUPPORTED: error = "UNSUPPORTED"; break;
		default: error = "UNKNOWN";
		}
		throw std::runtime_error("Framebuffer incomplete: " + std::string(error));
	}
	return status;

}

void SMFBO::Resize(int newWidth, int newHeight)
{
	// TODO : as always to be done for later
}
