#pragma once
#include <string>
#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
class SceneBase
{
public:

	SceneBase(const std::string& name, int width, int height, const glm::vec3& cameraPosition = glm::vec3(0.0f, 0.0f, 10.f)) :
		m_Name(name),
		m_Camera(width,height,cameraPosition) { }

	virtual ~SceneBase() = default;

	SceneBase(const SceneBase&) = delete;
	SceneBase& operator=(const SceneBase&) = delete;

	virtual void Load() = 0;
	virtual void Unload() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;
	virtual void OnImGuiRender() = 0;

	const std::string& GetName() { return m_Name; }
	Camera& GetCamera() { return m_Camera; }
	Scene& GetScene() { return m_Scene; }

protected:

	std::string m_Name;
	Scene m_Scene;
	Camera m_Camera;
	std::unique_ptr<Renderer> m_Renderer;
};

