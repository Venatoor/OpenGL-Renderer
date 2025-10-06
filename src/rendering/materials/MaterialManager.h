#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include "Material.h"
#include "shaderClass.h"


class MaterialManager
{
public:
	static MaterialManager& Get();

	std::shared_ptr<Material> CreateMaterial(const std::string name,
		Shader& shader,
		BlendMode blendmode = BlendMode::OPAQUE);

	std::shared_ptr<Material> GetMaterial(const std::string& name);

	std::vector<Shader*> GetMaterialShaders() const;

	void UpdateGlobalUniforms(const glm::mat4& view,
		const glm::mat4& projection,
		const glm::vec3& viewPos);

private:

	MaterialManager() = default;
	std::unordered_map<std::string, std::shared_ptr<Material>> materials;

};

