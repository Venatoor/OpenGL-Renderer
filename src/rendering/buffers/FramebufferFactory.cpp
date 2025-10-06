#include "FramebufferFactory.h"
#include "MSAAFBO.h"
#include "FBO.h"
#include "SMFBO.h"

std::unique_ptr<IFramebuffer> FramebufferFactory::Create(
    int width, int height,
    FramebufferType depthStencilType,
    bool useMultipleColorAttachments,
    bool createDepthStencil,
    bool depthAsTexture,
    size_t numColorAttachments,
    Type type,
    unsigned int samples)
{
    switch (type) {
    case Type::MSAA:
        return std::make_unique<MSAAFBO>(width, height, createDepthStencil, samples);
    case Type::STANDARD:
        return std::make_unique<FBO>(width, height, depthStencilType,
            useMultipleColorAttachments,std::nullopt, createDepthStencil,
            depthAsTexture, numColorAttachments);
    case Type::SM:
        return std::make_unique<SMFBO>(width, height);
    default:
        return std::make_unique<FBO>(width, height, depthStencilType,
            useMultipleColorAttachments,std::nullopt, createDepthStencil,
            depthAsTexture, numColorAttachments);
    }
}