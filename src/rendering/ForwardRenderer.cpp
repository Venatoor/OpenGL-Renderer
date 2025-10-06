#include "ForwardRenderer.h"
#include "BlendManager.h"
#include "Mesh.h"
#include "PointLight.h"
#include <algorithm>
#include "PostProcessEffect.h"

ForwardRenderer::ForwardRenderer(int width, int height) :
	m_width(width),
	m_height(height),
	shadowMap(2048, 2048),
	skybox({
		  "right.jpg", "left.jpg", "top.jpg",
		  "bottom.jpg", "front.jpg", "back.jpg"
		}, ShaderLibrary::Get().GetShader("skybox")),
    //postprocessingStack({ width, height, false, 4 })
    postProcessor(width, height, ShaderLibrary::Get().GetShader("postprocess"), 4)
{
}

void ForwardRenderer::Initialize(Scene& scene) {

	renderEntities = scene.GetRenderEntities();

    //postprocessingStack.AddEffect(std::make_unique<PostProcessEffect>("grayscale_pp", "grayscale", false));
}

void ForwardRenderer::Render(Scene& scene, Camera& camera, float deltaTime)
{
    ShadowPass();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    postProcessor.BeginRender();
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SkyboxPass(camera);

    GeometryPass(scene, camera);

    TransparentPass(scene, camera);

    BlendManager::SetBlendMode(BlendManager::NONE);

    postProcessor.EndRender();
    postProcessor.Render(deltaTime);


}

void ForwardRenderer::ShadowPass()
{
	if (!renderEntities.spot.empty()) {
        //this is a complete mess and wreck xD, shadows traumatised me for some reasons to be
        //updated later on 
        auto mainSpotLight = renderEntities.spot[0]->GetComponent<SpotLight>();
        shadowMap.UpdateFromSpotLight(*mainSpotLight);

        Shader& shadowShader = shaderLib.GetShader("shadowpass");
        SafeShaderActivate(shadowShader);
        shadowShader.SetMat4("lightViewProj", shadowMap.GetLightViewProj());

        shadowMap.BeginShadowPass();

        // Render all opaque and transparent objects to shadow map
        for (auto entity : renderEntities.opaqueEntities) {
            entity->Draw(shadowShader);
        }
        for (auto entity : renderEntities.alphaBlendEntities) {
            entity->Draw(shadowShader);
        }

        shadowMap.EndShadowPass();
        glViewport(0, 0, m_width, m_height);
	}
}

void ForwardRenderer::GeometryPass(Scene& scene, Camera& camera)
{
    BlendManager::SetBlendMode(BlendManager::NONE);

    for (auto& entity : renderEntities.opaqueEntities) {
        if (!entity->IsRenderable()) continue;

        auto* mesh = entity->GetComponent<Mesh>();
        if (!mesh) continue;

        auto material = mesh->GetMaterial();
        if (!material) continue;

        Shader& shader = material->GetShader();
        SafeShaderActivate(shader);

        ApplyCameraUniforms(shader, camera);

        //TODO : light passes 

        ApplyLightingUniforms(shader);

        ApplyCommonUniforms(shader, false);

        entity->Draw(shader);
    }
}

void ForwardRenderer::TransparentPass(Scene& scene, Camera& camera)
{
    BlendManager::SetBlendMode(BlendManager::ALPHA);

    auto transparentEntities = GetSortedTransparentEntities(scene, camera);

    for (auto& pair : transparentEntities) {
        if (!pair.second->IsRenderable()) continue;

        auto* mesh = pair.second->GetComponent<Mesh>();
        if (!mesh) continue;

        auto material = mesh->GetMaterial();
        if (!material) continue;

        Shader& shader = material->GetShader();
        SafeShaderActivate(shader);

        ApplyCameraUniforms(shader, camera);
        ApplyLightingUniforms(shader);
        ApplyCommonUniforms(shader, true);

        pair.second->Draw(shader);

    }

}

void ForwardRenderer::SkyboxPass(Camera& camera)
{
    glDepthMask(GL_FALSE);
    skybox.Draw(camera.GetViewMatrix(), camera.GetProjectionMatrix());
    glDepthMask(GL_TRUE);
}

void ForwardRenderer::ApplyCameraUniforms(Shader& shader, Camera& camera)
{
    camera.Matrix(shader, "camMatrix");
    shader.SetVec3("viewPos", camera.Position);
}

void ForwardRenderer::ApplyLightingUniforms(Shader& shader)
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

void ForwardRenderer::ApplyCommonUniforms(Shader& shader, bool hasAlpha)
{
    shader.SetBool("hasAlpha", hasAlpha);

    shadowMap.BindTexture(SHADOW_MAP_TEXTURE_UNIT);
    shader.SetInt("shadowMap", SHADOW_MAP_TEXTURE_UNIT);

    if (!renderEntities.directional.empty()) {
        shader.SetMat4("lightViewProj", shadowMap.GetLightViewProj());
    }

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.GetCubeMapID());
    shader.SetInt("skybox", 5);
}

std::vector<std::pair<float, Entity3D*>> ForwardRenderer::GetSortedTransparentEntities(Scene& scene,
    Camera& camera)
{
    std::vector<std::pair<float, Entity3D*>> transparentEntities;

    for (auto& entity : renderEntities.alphaBlendEntities) {
        if (!entity->IsRenderable()) continue;
        float distance = glm::distance(camera.Position, entity->GetTransform().GetPosition());
        transparentEntities.emplace_back(distance, entity);
    }

    std::sort(transparentEntities.begin(), transparentEntities.end(),
        [](const auto& a, const auto& b) {
            return a.first > b.first; // Sort back to front
        });

    return transparentEntities;
}

void ForwardRenderer::SafeShaderActivate(Shader& shader) {
    if (currentShader != &shader) {
        shader.Activate();
        currentShader = &shader;
    }
}