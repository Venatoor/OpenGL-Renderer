#include "SceneManager.h"

SceneManager& SceneManager::Get()
{
	static SceneManager instance;
	return instance;
}

void SceneManager::RegisterScene(const std::string& name, std::unique_ptr<SceneBase> scene)
{
	m_Scenes[name] = std::move(scene);
	m_ScenesNames.push_back(name);

	//=== Set First Scene as current if none set
	if (!m_CurrentScene) {
		SwitchScene(name);
	}
}

void SceneManager::SwitchScene(const std::string& name)
{
	if (m_Scenes.find(name) != m_Scenes.end()) {

		if (m_CurrentScene) {
			m_CurrentScene->Unload();
		}

		m_CurrentSceneName = name;
		m_CurrentScene = m_Scenes[name].get();
		m_CurrentScene->Load();

	}	
}

void SceneManager::ReloadCurrentScene()
{
	if (m_CurrentScene) {
		m_CurrentScene->Unload();
		m_CurrentScene->Load();
	}
}

void SceneManager::Update(float deltaTime)
{
	if (m_CurrentScene) {
		m_CurrentScene->Update(deltaTime);
	}
}

void SceneManager::Render()
{
	if (m_CurrentScene) {
		m_CurrentScene->Render();
	}
}

void SceneManager::OnImGUIRender()
{
	if (m_CurrentScene) {
		m_CurrentScene->OnImGuiRender();
	}
}
