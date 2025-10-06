#include "MaterialManager.h"

MaterialManager& MaterialManager::Get()
{
	static MaterialManager instance;
	return instance;
}

std::shared_ptr<Material> MaterialManager::CreateMaterial(const std::string name, Shader& shader, BlendMode blendmode)
{
	auto material = std::make_shared<Material>(shader, blendmode);
	materials[name] = material;
	return material;
}

std::shared_ptr<Material> MaterialManager::GetMaterial(const std::string& name)
{
	auto it = materials.find(name);
	if (it != materials.end()) {
		return it->second;
	}
}

std::vector<Shader*> MaterialManager::GetMaterialShaders() const
{
	std::vector<Shader*> shaders;
	for (const auto& pair : materials) {
		shaders.push_back(&pair.second->GetShader());
	}
	return shaders;
}

void MaterialManager::UpdateGlobalUniforms(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos)
{
}
