#include "DeferredRenderer.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "Mesh.h"

DeferredRenderer::DeferredRenderer(int width, int height, GlobalShadingModel initialModel) :
    m_width(width),
    m_height(height),
    skybox({
        "right.jpg", "left.jpg", "top.jpg",
        "bottom.jpg", "front.jpg", "back.jpg"
        }, ShaderLibrary::Get().GetShader("skybox")),
    postProcessor(width, height, ShaderLibrary::Get().GetShader("postprocess"), 4),
    gBuffer(),
    globalShadingModel(initialModel)
    /*
    gBuffer(width, height,:
        FramebufferType::DEPTH,  
        true,
        std::make_optional(std::array<Texture, FBO::MAX_ATTACHEMENTS>{  
        Texture(GL_TEXTURE_2D, GL_RGB16F, GL_RGB, GL_FLOAT, height, width),
        Texture(GL_TEXTURE_2D, GL_RGB16F, GL_RGB, GL_FLOAT, height, width),
        Texture(GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, height, width)
        }),
        true,
        false,
        3)
    */

{
    CreateGbuffer();
    SetupLightPassShaders();
    InitScreenQuad();
}

void DeferredRenderer::Render(Scene& scene, Camera& camera, float deltaTime)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    BlendManager::SetBlendMode(BlendManager::NONE);
    GeometryPass(scene, camera);
    LightingPass(scene, camera);
    SkyboxPass(camera);
}

void DeferredRenderer::Initialize(Scene& scene)
{
    renderEntities = scene.GetRenderEntities();
}

void DeferredRenderer::GeometryPass(Scene& scene, Camera& camera)
{
    gBuffer.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& entity : renderEntities.opaqueEntities) {
        if (!entity->IsRenderable()) continue;

        auto* mesh = entity->GetComponent<Mesh>();
        if (!mesh) continue;

        auto material = mesh->GetMaterial();
        if (!material) continue;

        Shader& shader = material->GetShader();
        SafeShaderActivate(shader);

        ApplyCameraUniforms(shader, camera);

        //Applying common uniforms from material struct
        entity->Draw(shader);   
    }
    gBuffer.Unbind();

}

void DeferredRenderer::LightingPass(Scene& scene, Camera& camera)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //This is temporary there might other rendering modes like NPR
    //Shader& lightingPassShader = shaderLib.GetShader("deferred_lighting");
    lightPassShader->Activate();

    switch (globalShadingModel) {
    case GlobalShadingModel::PBR:
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorAttachment(0).ID);
        lightPassShader->SetInt("gPosition", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorAttachment(1).ID);
        lightPassShader->SetInt("gNormal", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorAttachment(2).ID);
        lightPassShader->SetInt("gAlbedo", 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorAttachment(3).ID);
        lightPassShader->SetInt("gMetallicRoughness", 3);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorAttachment(4).ID);
        lightPassShader->SetInt("gAdditional", 4);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorAttachment(5).ID);
        lightPassShader->SetInt("gShadingModel", 5);
        break;

    case GlobalShadingModel::PHONG_BLINN:
    case GlobalShadingModel::NPR:
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorAttachment(0).ID);
        lightPassShader->SetInt("gPosition", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorAttachment(1).ID);
        lightPassShader->SetInt("gNormal", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorAttachment(2).ID);
        lightPassShader->SetInt("gAlbedoSpec", 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorAttachment(3).ID);
        lightPassShader->SetInt("gShadingModel", 3);
        break;
    }

    lightPassShader->SetVec3("viewPos", camera.Position);

    ApplyLightPassUniforms(*lightPassShader);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    gBuffer.BlitToDefaultFramebuffer(m_width, m_height, GL_DEPTH_BUFFER_BIT);
    gBuffer.Unbind();
    

}

void DeferredRenderer::SkyboxPass(Camera& camera)
{
    glDepthMask(GL_FALSE);
    skybox.Draw(camera.GetViewMatrix(), camera.GetProjectionMatrix());
    glDepthMask(GL_TRUE);
}

void DeferredRenderer::ApplyLightPassUniforms(Shader& shader)
{
    shader.SetInt("numDirLights", (int)renderEntities.directional.size());
    shader.SetInt("numPointLights", (int)renderEntities.point.size());
    shader.SetInt("numSpotLights", (int)renderEntities.spot.size());

    for (size_t i = 0; i < renderEntities.directional.size(); i++) {
        if (auto* light = renderEntities.directional[i]->GetComponent<DirectionalLight>()) {
            light->ApplyToShader(shader, "dirLights[" + std::to_string(i) + "]");
        }
    }
    for (size_t i = 0; i < renderEntities.point.size(); i++) {
        if (auto* light = renderEntities.point[i]->GetComponent<PointLight>()) {
            light->ApplyToShader(shader, "pointLights[" + std::to_string(i) + "]");
        }
    }
    for (size_t i = 0; i < renderEntities.spot.size(); i++) {
        if (auto* light = renderEntities.spot[i]->GetComponent<SpotLight>()) {
            light->ApplyToShader(shader, "spotLights[" + std::to_string(i) + "]");
        }
    }
}

void DeferredRenderer::ApplyCameraUniforms(Shader& shader, Camera& camera)
{
    camera.Matrix(shader, "camMatrix");
    shader.SetVec3("viewPos", camera.Position);
}

void DeferredRenderer::CreateGbuffer()
{
    std::array<Texture, FBO::MAX_ATTACHEMENTS> gBufferTextures;

    switch (globalShadingModel) {
    case GlobalShadingModel::PBR:
        gBufferTextures = {
                Texture(GL_TEXTURE_2D, GL_RGB16F, GL_RGB, GL_FLOAT, m_height, m_width),     // Position
                Texture(GL_TEXTURE_2D, GL_RGB16F, GL_RGB, GL_FLOAT, m_height, m_width),     // Normal
                Texture(GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, m_height, m_width), // Albedo
                Texture(GL_TEXTURE_2D, GL_RG16F, GL_RG, GL_FLOAT, m_height, m_width),       // Metallic + Roughness
                Texture(GL_TEXTURE_2D, GL_RG16F, GL_RG, GL_FLOAT, m_height, m_width),       // Additional properties ( specular, anisotropy, ao )
                Texture(GL_TEXTURE_2D, GL_R32I, GL_RED_INTEGER, GL_INT, m_height, m_width)  // Local Shading Model ID
        };
        break;
    case GlobalShadingModel::PHONG_BLINN:
        gBufferTextures = {
                Texture(GL_TEXTURE_2D, GL_RGB16F, GL_RGB, GL_FLOAT, m_height, m_width),     // Position
                Texture(GL_TEXTURE_2D, GL_RGB16F, GL_RGB, GL_FLOAT, m_height, m_width),     // Normal
                Texture(GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, m_height, m_width), // Diffuse + Specular
                Texture(GL_TEXTURE_2D, GL_R32I, GL_RED_INTEGER, GL_INT, m_height, m_width), // Local Shading Model ID
                Texture(), // Empty
                Texture()  // Empty
        };
        break;
    case GlobalShadingModel::NPR:
        gBufferTextures = {
            Texture(GL_TEXTURE_2D, GL_RGB16F, GL_RGB, GL_FLOAT, m_height, m_width),     // Position
            Texture(GL_TEXTURE_2D, GL_RGB16F, GL_RGB, GL_FLOAT, m_height, m_width),     // Normal
            Texture(GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, m_height, m_width), // Base color
            Texture(GL_TEXTURE_2D, GL_R32I, GL_RED_INTEGER, GL_INT, m_height, m_width), // Local Shading Model ID
            Texture(), // Empty for now will add outlines and strokes later on 
            Texture()  // Empty
        };
        break;
    }

    gBuffer = FBO(m_width, m_height, FramebufferType::DEPTH, true, gBufferTextures, true, false, 6);

}

void DeferredRenderer::SetupLightPassShaders()
{
    std::string lightingShaderName;

    switch (globalShadingModel) {
    case GlobalShadingModel::PBR:
        lightingShaderName = "deferred_pbr_lighting";
        break;
    case GlobalShadingModel::PHONG_BLINN:
        lightingShaderName = "deferred_phong_lighting";
        break;
    case GlobalShadingModel::NPR:
        lightingShaderName = "deferred_npr_lighting";
        break;
    }

    lightPassShader = &shaderLib.GetShader(lightingShaderName);
}

void DeferredRenderer::SafeShaderActivate(Shader& shader)
{
    if (currentShader != &shader) {
        shader.Activate();
        currentShader = &shader;
    }
}

void DeferredRenderer::InitScreenQuad()
{
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}
    