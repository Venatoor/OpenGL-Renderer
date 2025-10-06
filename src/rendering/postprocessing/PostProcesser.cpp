#include "PostProcesser.h"
#include "iostream"
#include "MSAAFBO.h"
#include "FBO.h"
#include <array>

PostProcessor::PostProcessor(int width, int height, Shader& shader, unsigned int msaaSamples ) :
	postProcessingShader(shader),
	msaaSamples(msaaSamples)
{
    msaaFBO = FramebufferFactory::Create(width, height, FramebufferType::DEPTH_STENCIL,false,true,false,0,
        FramebufferFactory::Type::MSAA, msaaSamples);

    resolvedFBO = std::make_unique<FBO>(
        width, height,
        FramebufferType::DEPTH_STENCIL,
        false,std::nullopt, true, false, 1
    );

	InitRenderData();
}

PostProcessor::~PostProcessor()
{
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}


void PostProcessor::InitRenderData()
{
    GLfloat vertices[] = {
        // Pos        // Tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

    glBindVertexArray(0);

}


void PostProcessor::BeginRender()
{
    if (msaaSamples > 1) {
        msaaFBO->Bind();
        glEnable(GL_MULTISAMPLE);
    }
    else {
        resolvedFBO->Bind();
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessor::EndRender()
{
    if (msaaSamples > 1) {
        auto msaaBuffer = dynamic_cast<MSAAFBO*>(msaaFBO.get());
        if (msaaBuffer) {
            GLuint tempFBO;
            glGenFramebuffers(1, &tempFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, tempFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D,
                msaaBuffer->GetColorAttachment().ID, 0);

            // Resolve to our texture
            msaaBuffer->ResolveToTexture(tempFBO);

            // Cleanup
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &tempFBO);
        }
        glDisable(GL_MULTISAMPLE);
        msaaFBO->Unbind();
    }
    else {
        resolvedFBO->Unbind();
    }
    
}

void PostProcessor::Render(float time)
{
    postProcessingShader.Activate();
    postProcessingShader.SetUInt("activeEffects", activeEffects);
    postProcessingShader.SetBool("applyGamma", true);

    // Bind the texture from the FBO
    glActiveTexture(GL_TEXTURE0);
    if (msaaSamples > 1) {
        dynamic_cast<MSAAFBO*>(msaaFBO.get())->GetColorAttachment().Bind();
    }
    else {
        
    }
    postProcessingShader.SetInt("scene", 0);

    // Render quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessor::SetMSAASamples(unsigned int samples)
{
    if (samples == msaaSamples) return;

    msaaSamples = samples;
    if (msaaSamples > 1) {
        msaaFBO = FramebufferFactory::Create(resolvedFBO->GetWidth(), resolvedFBO->GetHeight(), FramebufferType::DEPTH_STENCIL, false, true, false, 0,
            FramebufferFactory::Type::MSAA, msaaSamples);
    }
}