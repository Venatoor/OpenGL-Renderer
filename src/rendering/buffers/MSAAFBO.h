#pragma once
#include "IFramebuffer.h"
#include "optional"

class MSAAFBO : public IFramebuffer
{
public:
    MSAAFBO(int width, int height, bool createDepthStencil, unsigned int samples);
    ~MSAAFBO();

    void Bind() const override;
    void Unbind() const override;

    const Texture& GetColorAttachment() const  { return resolvedTexture; }
    bool HasDepthStencil() const  { return hasDepthStencil; }
    int GetWidth() const override { return width; }
    int GetHeight() const override { return height; }

    bool CheckStatus() const override;
    void Resize(int newWidth, int newHeight) override;


    void ResolveToTexture(GLuint targetFBO);

    GLuint GetID() { return msaaFBO; }

private:
    void CreateMultisampleColorBuffer();
    void CreateDepthStencilBuffer();

    GLuint msaaFBO;
    GLuint msaaColorBuffer;
    GLuint rbo = 0;
    int width;
    int height;
    unsigned int samples;
    bool hasDepthStencil = false;
    Texture resolvedTexture;

};

