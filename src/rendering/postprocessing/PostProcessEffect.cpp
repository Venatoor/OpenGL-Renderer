#include "PostProcessEffect.h"
#include "ShaderLibrary.h"

PostProcessEffect::PostProcessEffect(const std::string& name, const std::string& shaderName, bool requiresDepth) : 
	shaderName(shaderName),
	effectName(name),
	requiresDepth(requiresDepth),
	effectShader(GetShader(shaderName))
{
    InitRenderData();
}

PostProcessEffect::~PostProcessEffect()
{
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
}

void PostProcessEffect::Apply(GLuint texture, bool lasteffect)
{
    effectShader.Activate();
    SetupUniforms();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    RenderQuad();
}

void PostProcessEffect::SetupUniforms()
{
	effectShader.SetBool("applyGamma", true);
	effectShader.SetInt("scene", 0);
}

Shader& PostProcessEffect::GetShader(const std::string& shaderName)
{
	return ShaderLibrary::Get().GetShader(shaderName);
}

void PostProcessEffect::RenderQuad()
{
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessEffect::InitRenderData()
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
