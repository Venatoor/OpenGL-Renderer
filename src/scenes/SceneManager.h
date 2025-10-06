#pragma once	
#include "SceneBase.h"
#include <unordered_map>
#include <vector>
#include <memory>

class SceneManager
{
public:

	static SceneManager& Get();

	void RegisterScene(const std::string& name, std::unique_ptr<SceneBase> scene);
	void SwitchScene(const std::string& name);
	void ReloadCurrentScene();

	SceneBase* GetCurrentScene() { return m_CurrentScene; }
	const std::vector<std::string>& GetSceneNames() const { return m_ScenesNames; }

	void Update(float deltaTime);
	void Render();
	void OnImGUIRender();

private:

	SceneManager() = default;
	std::unordered_map<std::string, std::unique_ptr<SceneBase>> m_Scenes;
	std::vector<std::string> m_ScenesNames;

	SceneBase* m_CurrentScene = nullptr;
	std::string m_CurrentSceneName;

};

