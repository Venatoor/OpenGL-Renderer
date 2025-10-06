#pragma once
#include "SceneBase.h"
#include "Model.h"


class BasicRenderingScene : public SceneBase
{
public:
	
	BasicRenderingScene() :
		SceneBase(name, width, height, glm::vec3(0.0f, 0.0f, 10.0f)),
		magnumModel("D:/source/Magnum.fbx") 
	{
	}

	void Load() override;
	void Unload() override;
	void Update(float deltaTime) override;
	void Render() override;
	void OnImGuiRender() override;

private:

	void SetupShaders();
	void SetupMaterials();
	void SetupEntities();
	void SetupLights();

	//======== ENTITIES
	Entity3D* baseCube = nullptr;
	Entity3D* secondCube = nullptr;
	Entity3D* backQuad1 = nullptr;
	Entity3D* backQuad2 = nullptr;
	Entity3D* floor = nullptr;
	Entity3D* magnum = nullptr;

	Entity3D* pointLight = nullptr;
	Entity3D* spotLight = nullptr;
	Entity3D* directionalLight = nullptr;

	//======== MATERIALS

	std::shared_ptr<Material> cubeMaterial = nullptr;
	std::shared_ptr<Material> transparentMaterial = nullptr;
	std::shared_ptr<Material> magnumMaterial = nullptr;

	//======== MODELS
	Model magnumModel;
	
	inline static const std::string name = "Basic Rendering";
	static constexpr unsigned int width = 1900;
	static constexpr unsigned int height = 1080;

	//======= MATERIAL PROPERTIES

	// UI controls
	glm::vec3 m_CubePosition{ 0.0f };
	glm::vec3 m_CubeRotation{ 0.0f };
	glm::vec3 m_CubeScale{ 1.0f };

	glm::vec3 m_SpecularColor{ 0.5f };
	float m_AmbientStrength = 0.1f;
	float m_Shininess = 32.0f;
	bool m_UseDiffuse = false;
	bool m_UseSpecular = false;
	bool m_UseEmissive = false;
	bool m_UseNormalMap = false;
	float m_Reflectivity = 0.0f;
	float m_RefractionRatio = 1.52f;
	bool m_UseReflection = false;
	bool m_UseRefraction = false;

};

