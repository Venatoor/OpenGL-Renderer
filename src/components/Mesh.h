#pragma once
#include<glm/glm.hpp>
#include<vector>
#include<string>
#include"shaderClass.h"
#include"Texture.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"	
#include "Material.h"
#include "Component.h"
#include "MeshData.h"

class Mesh : public Component
{
public:

	enum class PrimitiveType { CUSTOM, CUBE, QUAD};


	Mesh(std::shared_ptr<MeshData> sharedData);
	

	void OnRender(Shader& shader, const glm::mat4& modelMatrix) override;


	// Material setter
	void SetMaterial(const std::shared_ptr<Material>& newMaterial) {
		material = newMaterial;
	}

	std::shared_ptr<Material> GetMaterial() const {
		return material;
	}

	std::shared_ptr<MeshData> GetMeshData() const {
		return meshData;
	}


private:
	
	std::shared_ptr<MeshData> meshData;
	std::shared_ptr<Material> material;

};

