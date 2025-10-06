#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "TerrainBlendModes.h"
#include <vector>

class Heightmap;

struct TerrainLayer {

	std::string name;
	//material which i will add later :)
	std::shared_ptr<Heightmap> heightmap;
	TerrainBlendMode blendMode;

	float weight = 1.0f;
	float heightScale = 1.0f;
	float heightOffset = 1.0f;

	glm::vec2 scale{ 1.0f, 1.0f };
	glm::vec2 offset{ 0.0f, 0.0f };


};

struct TerrainMeshData {

	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	void Clear() {
		texCoords.clear();
		normals.clear();
		vertices.clear();
		indices.clear();
	}

	bool HasData() const {
		return !vertices.empty() && !indices.empty();
	}

};

