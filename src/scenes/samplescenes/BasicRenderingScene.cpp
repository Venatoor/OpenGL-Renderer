#include "BasicRenderingScene.h"
#include "ShaderLibrary.h"
#include "MaterialManager.h"
#include "CommonRessources.h"
#include "Mesh.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "ForwardRenderer.h"
#include <imgui.h>


void BasicRenderingScene::Load()
{

    SetupShaders();
    SetupMaterials();
    SetupEntities();
    SetupLights();

    m_Renderer = std::make_unique<ForwardRenderer>(width, height);

    auto* forwardRenderer = dynamic_cast<ForwardRenderer*>(m_Renderer.get());
    if (forwardRenderer) {
        forwardRenderer->Initialize(m_Scene);
    }


    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

}

void BasicRenderingScene::Unload()
{
    m_Renderer.reset();
}

void BasicRenderingScene::Update(float deltaTime)
{
   
   // m_Camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
    m_Scene.OnUpdate(deltaTime);

    if (cubeMaterial) {
        cubeMaterial->GetShader().Activate();
        cubeMaterial->GetShader().SetFloat("material.ambient", m_AmbientStrength);
        cubeMaterial->GetShader().SetFloat("material.shininess", m_Shininess);
        cubeMaterial->GetShader().SetVec3("material.specular", m_SpecularColor);
        cubeMaterial->GetShader().SetBool("material.useDiffuseMap", m_UseDiffuse);
        cubeMaterial->GetShader().SetBool("material.useSpecularMap", m_UseSpecular);
        cubeMaterial->GetShader().SetBool("material.useEmissiveMap", m_UseEmissive);
        cubeMaterial->GetShader().SetBool("material.useNormalMap", m_UseNormalMap);
        cubeMaterial->GetShader().SetBool("material.useReflection", m_UseReflection);
        cubeMaterial->GetShader().SetBool("material.useRefraction", m_UseRefraction);
        cubeMaterial->GetShader().SetFloat("material.reflectivity", m_Reflectivity);
        cubeMaterial->GetShader().SetFloat("material.refractionRatio", m_RefractionRatio);
    }

    if (transparentMaterial) {
        transparentMaterial->GetShader().Activate();
        transparentMaterial->GetShader().SetFloat("material.ambient", m_AmbientStrength);
        transparentMaterial->GetShader().SetFloat("material.shininess", m_Shininess);
        transparentMaterial->GetShader().SetVec3("material.specular", m_SpecularColor);
        transparentMaterial->GetShader().SetBool("material.useDiffuseMap", m_UseDiffuse);
        transparentMaterial->GetShader().SetBool("material.useSpecularMap", false);
        transparentMaterial->GetShader().SetBool("material.useEmissiveMap", false);
    }

    if (magnumMaterial) {
        magnumMaterial->GetShader().Activate();
        magnumMaterial->GetShader().SetFloat("material.ambient", m_AmbientStrength);
        magnumMaterial->GetShader().SetFloat("material.shininess", m_Shininess);
        magnumMaterial->GetShader().SetVec3("material.specular", m_SpecularColor);
        magnumMaterial->GetShader().SetBool("material.useDiffuseMap", m_UseDiffuse);
        magnumMaterial->GetShader().SetBool("material.useSpecularMap", m_UseSpecular);
        magnumMaterial->GetShader().SetBool("material.useEmissiveMap", m_UseEmissive);
        magnumMaterial->GetShader().SetBool("material.useNormalMap", m_UseNormalMap);
        magnumMaterial->GetShader().SetBool("material.useReflection", m_UseReflection);
        magnumMaterial->GetShader().SetBool("material.useRefraction", m_UseRefraction);
        magnumMaterial->GetShader().SetFloat("material.reflectivity", m_Reflectivity);
        magnumMaterial->GetShader().SetFloat("material.refractionRatio", m_RefractionRatio);
    }
}

void BasicRenderingScene::Render()
{
    if (m_Renderer) {
        auto* forwardRenderer = dynamic_cast<ForwardRenderer*>(m_Renderer.get());
        if (forwardRenderer) {
            forwardRenderer->Render(m_Scene, m_Camera, 0.0f);
        }
    }
}

void BasicRenderingScene::OnImGuiRender()
{
    ImGui::Begin("Basic Rendering Scene");

    ImGui::Text("Material Controls");
    ImGui::Checkbox("Use Diffuse Texture", &m_UseDiffuse);
    ImGui::Checkbox("Use Specular Texture", &m_UseSpecular);
    ImGui::Checkbox("Use Normal Map", &m_UseNormalMap);

    ImGui::ColorEdit3("Specular Color", glm::value_ptr(m_SpecularColor));
    ImGui::DragFloat("Ambient Strength", &m_AmbientStrength, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Shininess", &m_Shininess, 1.0f, 1.0f, 256.0f);

    ImGui::End();
}

void BasicRenderingScene::SetupShaders()
{
    ShaderLibrary& shaderLib = ShaderLibrary::Get();
    shaderLib.AddShader("default", "default.vert", "default.frag");
    shaderLib.AddShader("postprocess", "post_process.vert", "post_process.frag");
    shaderLib.AddShader("shadowpass", "shadowpass.vert", "shadowpass.frag");
    shaderLib.AddShader("skybox", "skybox.vert", "skybox.frag");
    shaderLib.AddShader("transclucent", "default.vert", "default.frag");

    shaderLib.AddShader("grayscale", "post_process.vert", "grayscale_pp.frag");
    shaderLib.AddShader("chromatic_aberration", "post_process.vert", "chromaticab_pp.frag");
    shaderLib.AddShader("edge_detection", "post_process.vert", "edgedetect_pp.frag");
    shaderLib.AddShader("invert", "post_process.vert", "invert_pp.frag");
    shaderLib.AddShader("blur", "post_process.vert", "blur_pp.frag");


    Shader& shadowPassShader = shaderLib.GetShader("shadowpass");
    Shader& defaultShader = shaderLib.GetShader("default");
    Shader& postprocessShader = shaderLib.GetShader("postprocess");
    Shader& skyboxShader = shaderLib.GetShader("skybox");
    Shader& transclucentShader = shaderLib.GetShader("transclucent");
}

void BasicRenderingScene::SetupMaterials()
{

    auto& defaultShader = ShaderLibrary::Get().GetShader("default");
    auto& transclucentShader = ShaderLibrary::Get().GetShader("transclucent");


    //Creation of Textures 
    std::shared_ptr<Texture> containerDiffuseTex = std::make_shared<Texture>(
        "containerDiffuse.png", Texture::Type::DIFFUSE, GL_TEXTURE_2D, GL_TEXTURE0, GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE, 3);
    std::shared_ptr<Texture> containerSpecularTex = std::make_shared<Texture>(
        "containerSpecular.png", Texture::Type::SPECULAR, GL_TEXTURE_2D, GL_TEXTURE1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, 3);
    std::shared_ptr<Texture> containerEmissiveTex = std::make_shared<Texture>(
        "containerEmissive.png", Texture::Type::EMISSIVE, GL_TEXTURE_2D, GL_TEXTURE2, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, 3);
    std::shared_ptr<Texture> containerNormalTex = std::make_shared<Texture>(
        "containerNormal.png", Texture::Type::NORMAL, GL_TEXTURE_2D, GL_TEXTURE3, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, 3);
    std::shared_ptr<Texture> glassDiffuseTex = std::make_shared<Texture>(
        "blending_transparent_window.png", Texture::Type::DIFFUSE, GL_TEXTURE_2D, GL_TEXTURE0, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, 4);

    stbi_set_flip_vertically_on_load(true);
    std::shared_ptr<Texture> magnumDiffuseTex = std::make_shared<Texture>(
        "D:/Source/diffuse.png", Texture::Type::DIFFUSE, GL_TEXTURE_2D, GL_TEXTURE0, GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE, 3);
    std::shared_ptr<Texture> magnumNormalTex = std::make_shared<Texture>(
        "D:/Source/normal.png", Texture::Type::NORMAL, GL_TEXTURE_2D, GL_TEXTURE3, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, 3);
    stbi_set_flip_vertically_on_load(false);

    MaterialManager& materialLib = MaterialManager::Get();

    materialLib.CreateMaterial("cubeMaterial", defaultShader, BlendMode::OPAQUE);
    materialLib.CreateMaterial("transparentMaterial", transclucentShader, BlendMode::ALPHA);
    materialLib.CreateMaterial("magnumMaterial", defaultShader, BlendMode::OPAQUE);

    //Creation of Materials + Textures Attribution 
    cubeMaterial = materialLib.GetMaterial("cubeMaterial");
    cubeMaterial->SetTexture(Texture::Type::DIFFUSE, containerDiffuseTex);
    cubeMaterial->SetTexture(Texture::Type::SPECULAR, containerSpecularTex);
    cubeMaterial->SetTexture(Texture::Type::NORMAL, containerNormalTex);
    cubeMaterial->SetTexture(Texture::Type::EMISSIVE, containerEmissiveTex);

    transparentMaterial = materialLib.GetMaterial("transparentMaterial");
    transparentMaterial->SetTexture(Texture::Type::DIFFUSE, glassDiffuseTex);

    magnumMaterial = materialLib.GetMaterial("magnumMaterial");
    magnumMaterial->SetTexture(Texture::Type::DIFFUSE, magnumDiffuseTex);
    magnumMaterial->SetTexture(Texture::Type::NORMAL, magnumNormalTex);
}

void BasicRenderingScene::SetupEntities()
{   
    //Base Cube
    baseCube = m_Scene.CreateEntity("BaseCube");
    baseCube->GetTransform().SetPosition({ 0.0f, 0.0f, 0.0f });
    Mesh* baseCubeMesh = baseCube->AddComponent<Mesh>(CommonRessources::GetCubeMesh());
    baseCubeMesh->SetMaterial(cubeMaterial);

    //Second Cube
    secondCube = m_Scene.CreateEntity("SecondCube");
    secondCube->GetTransform().SetPosition({ 2.0f, 0.0f, 0.0f });
    Mesh* secondCubeMesh = secondCube->AddComponent<Mesh>(CommonRessources::GetCubeMesh());
    secondCubeMesh->SetMaterial(cubeMaterial);

    //Back Quad 1 
    backQuad1 = m_Scene.CreateEntity("BackQuadFirst");
    backQuad1->GetTransform().SetPosition({ 0.0f, 0.0f, -3.0f });
    backQuad1->GetTransform().SetScale({ 2.0f, 2.0f, 1.0f });
    Mesh* backQuad1Mesh = backQuad1->AddComponent<Mesh>(CommonRessources::GetVerticalQuadMesh());
    backQuad1Mesh->SetMaterial(transparentMaterial);

    //Back Quad 2 
    backQuad2 = m_Scene.CreateEntity("BackQuadSecond");
    backQuad2->GetTransform().SetPosition({ 0.0f, 0.0f, -5.0f });
    backQuad2->GetTransform().SetScale({ 2.0f, 2.0f, 1.0f });
    Mesh* backQuad2Mesh = backQuad2->AddComponent<Mesh>(CommonRessources::GetVerticalQuadMesh());
    backQuad2Mesh->SetMaterial(transparentMaterial);
    //Floor
    floor = m_Scene.CreateEntity("Floor");
    floor->GetTransform().SetPosition({ 0.0f, -1.0f, 0.0f });
    floor->GetTransform().SetScale({ 1.0f, 1.0f, 1.0f });
    Mesh* floorMesh = floor->AddComponent<Mesh>(CommonRessources::GetPlaneMesh());
    floorMesh->SetMaterial(cubeMaterial);
    //Magnum
    magnum = m_Scene.CreateEntity("Magnum");
    magnum->GetTransform().SetPosition({ 0.0f, 2.0f, 0.0f });
    magnum->GetTransform().SetScale({ 2.f, 2.f, 2.f });
    Mesh* magnumMesh = magnum->AddComponent<Mesh>(magnumModel.meshes[0].GetMeshData());
    magnumMesh->SetMaterial(magnumMaterial);

    

}

void BasicRenderingScene::SetupLights()
{
    //Point Light
    pointLight = m_Scene.CreateEntity("PointLight");
    PointLight* pointLightComp = pointLight->AddComponent<PointLight>();
    pointLightComp->position = glm::vec3(0.0f, 1.1f, 0.0f);
    pointLightComp->color = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLightComp->radius = 10.0f;
    pointLightComp->intensity = 0.f;

    //Spot Light
    spotLight = m_Scene.CreateEntity("SpotLight");
    SpotLight* spotLightComp = spotLight->AddComponent<SpotLight>();
    spotLightComp->position = glm::vec3(2.0f, 2.0f, 2.0f);
    spotLightComp->direction = glm::vec3(-1.0f, -1.0f, -1.0f);
    spotLightComp->color = glm::vec3(0.8f, 0.8f, 1.0f);
    spotLightComp->radius = 10.0f;
    spotLightComp->intensity = 0.f;

    //Directional Light
    directionalLight = m_Scene.CreateEntity("DirectionalLight");
    DirectionalLight* directionalLightComp = directionalLight->AddComponent<DirectionalLight>();
    directionalLightComp->direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    directionalLightComp->color = glm::vec3(1.f, 1.f, 1.f);
    directionalLightComp->intensity = 1.f;;
}
