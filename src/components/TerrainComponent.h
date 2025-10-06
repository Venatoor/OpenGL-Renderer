#pragma once
#include "Component.h"
#include "HeightMap.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shaderClass.h"
#include <memory>
#include "TerrainStructs.h"

class TerrainComponent : public Component
{
public:

	TerrainComponent();
	~TerrainComponent();

	// === layers
	void PushLayer(const TerrainLayer& layer);
	void PopLayer();
	void ClearLayers();

	// setters and getters

	const TerrainMeshData& GetMeshData() const { return meshData; }
	const std::vector<TerrainLayer>& GetLayers() const { return layers; }
	int GetVertexCount() { return meshData.vertices.size() / 3; }
	int GetTriangleCount() { return meshData.indices.size() / 3; }
	bool ContainsHeightmap() { return baseHeightmap != nullptr; }

	// mesh preparation 

	void GenerateIndices();
	void GenerateVertices();
	void GenerateTexCoords();
	void GenerateNormals();

	void GenerateMesh();

	// main operations
		
	void GenerateFromHeightmap(const Heightmap* heightmap, float yScale, float yShift);
	
	//clearly this shouldn't be here but wtv XDDD ill mark it with TODO
	float Lerp(float a, float b, float t);
	float Lerp(float a, float b, float t) const;

	void OnRender(Shader& shader, const glm::mat4& modelMatrix) override;

private:

	std::vector<TerrainLayer> layers;
	std::shared_ptr<Heightmap> baseHeightmap;
	TerrainMeshData meshData;

	float SampleHeightmap(Heightmap* heightmap, int x, int y, const TerrainLayer* layer) const;
	float CalculateFinalHeight(int x, int y) const;

};