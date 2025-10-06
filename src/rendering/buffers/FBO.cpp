#include "FBO.h"

FBO::FBO(int width, int height, FramebufferType depthStencilType,
	bool useMultipleColorAttachments, std::optional<std::array<Texture, MAX_ATTACHEMENTS>>&& textures 
	, bool createDepthStencil, bool depthAsTexture
	,size_t numColorAttachments) :

	height(height),	
	width(width),
	depthType(depthStencilType),
	createDepthStencil(createDepthStencil),
	depthAsTexture(depthAsTexture),
	colorAttachments()
{

	if (textures) {
		for (size_t i = 0; i < numColorAttachments; ++i) {
			colorAttachments[i] = std::move(textures.value()[i]);
		}
	}


	if (width <= 0 || height <= 0) {
		std::cerr << "ERROR: Invalid FBO dimensions " << width << "x" << height
			<< " - must be positive values!" << std::endl;
		throw std::runtime_error("Invalid FBO dimensions");
	}

	std::cout << "Creating FBO (" << width << "x" << height << ")" << std::endl;

	glGenFramebuffers(1, &ID);

	std::cout << "Generated FBO with ID: " << ID << std::endl;

	Bind();

	std::cout << "Initializing color attachments..." << std::endl;

	InitializeColorAttachments(useMultipleColorAttachments, numColorAttachments);

	if (createDepthStencil) {
		std::cout << "Attaching depth/stencil buffer..." << std::endl;
		AttachDepthStencil(depthStencilType);
	}

	if (!CheckStatus()) {
		std::cout << "Standard FBO - Framebuffer not complete !";
	}

	Unbind();
	std::cout << "FBO creation completed" << std::endl;
}

FBO::~FBO()
{
	if (hasDepthStencil && rbo != 0) { 
		glDeleteRenderbuffers(1, &rbo); 
	}
	glDeleteFramebuffers(1, &ID);
}

void FBO::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void FBO::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::BlitToFBO(GLuint targetFBO, GLbitfield mask) const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO);
	glBlitFramebuffer(0, 0, width, height,
		0, 0, width, height,
		mask, GL_LINEAR);
}

void FBO::BlitToDefaultFramebuffer(int windowWidth, int windowHeight, GLbitfield mask) const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, width, height,
		0, 0, windowWidth, windowHeight,
		mask, GL_LINEAR);
}

const Texture& FBO::GetColorAttachment(size_t index) const
{
	if (colorAttachment) {
		return *colorAttachment;
	}
	else {
		if (index >= colorAttachments.size()) {
			throw std::out_of_range("Color attachment index out of range");
		}
		return colorAttachments[index];
	}
}

size_t FBO::GetColorAttachmentCount() const
{
	return colorAttachment ? 1 : colorAttachments.size();
}


bool FBO::CheckStatus() const
{
	Bind();
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer error: " << status << " (";
		switch (status) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: std::cout << "INCOMPLETE_ATTACHMENT"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: std::cout << "MISSING_ATTACHMENT"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: std::cout << "INCOMPLETE_DRAW_BUFFER"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: std::cout << "INCOMPLETE_READ_BUFFER"; break;
		case GL_FRAMEBUFFER_UNSUPPORTED: std::cout << "UNSUPPORTED"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: std::cout << "INCOMPLETE_MULTISAMPLE"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: std::cout << "INCOMPLETE_LAYER_TARGETS"; break;
		default: std::cout << "UNKNOWN"; break;
		}
		std::cout << ")" << std::endl;

		// Print attachment status
		GLint colorAttachments = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &colorAttachments);
		std::cout << "Max color attachments: " << colorAttachments << std::endl;

		for (int i = 0; i < colorAttachments; ++i) {
			GLint type = 0;
			glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &type);
			if (type != GL_NONE) {
				std::cout << "Color attachment " << i << " is attached (type: " << type << ")" << std::endl;
			}
		}


		GLint depthType = 0, stencilType = 0;
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &depthType);
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
			GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &stencilType);
		std::cout << "Depth attachment: " << (depthType != GL_NONE ? "present" : "missing") << std::endl;
		std::cout << "Stencil attachment: " << (stencilType != GL_NONE ? "present" : "missing") << std::endl;
	}

	Unbind();
	return status == GL_FRAMEBUFFER_COMPLETE;
}

void FBO::Resize(int newWidth, int newHeight)
{
	//l3gz
	//will write this variable function when i will need it xD
}

void FBO::InitializeColorAttachments( bool useMultiple, size_t count)
{
	if (useMultiple) {

		// I used these before when i did dynamic allocation, switched to static arrays for more performance 

		//colorAttachments.reserve(count);
		//std::vector<GLenum> drawBuffers;
		//drawBuffers.reserve(count);

		for (size_t i = 0; i < count; i++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
				GL_TEXTURE_2D, colorAttachments[i].ID, 0);
			drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
		}

		glDrawBuffers(static_cast<GLsizei>(count), drawBuffers.data());
	}
	else {
		colorAttachment.emplace(GL_TEXTURE_2D, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE, height, width);

		std::cout << "Single color attachment texture ID: " << colorAttachment->ID << std::endl;

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, colorAttachment->ID, 0);


		glDrawBuffer(GL_COLOR_ATTACHMENT0);

	}
}

void FBO::AttachDepthStencil(FramebufferType type)
{
	if (hasDepthStencil) {
		if (rbo ) glDeleteRenderbuffers(1, &rbo);
		if (depthTexture) depthTexture.reset();
	}

	switch (type) {
	case FramebufferType::DEPTH:
		if (depthAsTexture) {
			depthTexture.emplace(GL_TEXTURE_2D, GL_DEPTH_COMPONENT,
				GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT, height, width);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D, depthTexture->ID, 0);
		}
		else {
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);

			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_RENDERBUFFER, rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
		break;

	case FramebufferType::DEPTH_STENCIL:
		if (depthAsTexture) {
			depthTexture.emplace(GL_TEXTURE_2D, GL_DEPTH_STENCIL,
				GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8, height, width);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
				GL_TEXTURE_2D, depthTexture->ID, 0);
		}
		else {
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
				GL_RENDERBUFFER, rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
		break;
	case FramebufferType::STENCIL:  // Stencil-only requires RBO
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER, rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		break;

	default:
		break;

	}

	hasDepthStencil = true;
	
}

