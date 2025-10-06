#pragma once
#include "IFramebuffer.h"
#include <memory>

class FramebufferFactory
{
public:
    enum class Type {
        STANDARD,
        MSAA,
        SM
    };

    static std::unique_ptr<IFramebuffer> Create(
        int width, int height,
        FramebufferType depthStencilType,
        bool useMultipleColorAttachments,
        bool createDepthStencil,
        bool depthAsTexture,
        size_t numColorAttachments,
        Type type,
        unsigned int samples);
};

