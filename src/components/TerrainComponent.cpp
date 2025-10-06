#include "TerrainComponent.h"

TerrainComponent::TerrainComponent()
{
}

TerrainComponent::~TerrainComponent()
{
}

void TerrainComponent::PushLayer(const TerrainLayer& layer)
{
	layers.push_back(layer);
	std::cout << "Pushed the layer : " << layer.name << " with the blendmode : " << (int)layer.blendMode << std::endl;
}

void TerrainComponent::PopLayer()
{
	if (layers.empty()) return;
	layers.pop_back();
	std::cout << "Removed the layer : " << layers.back().name << std::endl;

}

void TerrainComponent::ClearLayers()
{
	layers.clear();
	std::cout << "Cleared all layers" << std::endl;
}

void TerrainComponent::GenerateIndices()
{
	if (!baseHeightmap) return;

	const auto& info = baseHeightmap->GetHeightmapSize();
	
	const unsigned int NUM_STRIPS = info.height - 1;
	const unsigned int NUM_VERTS_PER_STRIP = info.width * 2;

	const int numberOfIndices = NUM_STRIPS * NUM_VERTS_PER_STRIP;

	meshData.indices.clear();
	meshData.indices.reserve(numberOfIndices * 2);

	for (unsigned int i = 0; i < NUM_STRIPS; i++) {
		for (unsigned int j = 0; j < info.width; j++) {
			meshData.indices.push_back(j + info.width * (i + 0));
			meshData.indices.push_back(j + info.width * (i + 1));
		}
	}

}

void TerrainComponent::GenerateVertices()
{
	if (!baseHeightmap) return;

	const auto& info = baseHeightmap->GetHeightmapSize();
	const int totalVertices = info.width * info.height;

	meshData.vertices.clear();
	meshData.vertices.reserve(totalVertices * 3 + 64);

	for (int y = 0; y < info.height; y++) {
		for (int x = 0; x < info.width; x++) {
			float worldHeight = CalculateFinalHeight(x, y);

			float worldX = (-(info.height / 2.0f) + y);
			float worldZ = (-(info.width / 2.0f) + x);

			meshData.vertices.push_back(worldX);
			meshData.vertices.push_back(worldHeight);
			meshData.vertices.push_back(worldZ);
		}
	}
	meshData.vertices.shrink_to_fit();
}

void TerrainComponent::GenerateTexCoords()
{
	if (!baseHeightmap) return;

	const auto& info = baseHeightmap->GetHeightmapSize();
	const int totalVertices = info.width * info.height;

	meshData.texCoords.clear();
	meshData.texCoords.reserve(totalVertices * 2);

	for (int j = 0; j < info.height; j++) {
		for (int i = 0; i < info.width; i++) {
			float u = static_cast<float>(i) / (info.width - 1);
			float v = static_cast<float>(j) / (info.height - 1);

			meshData.texCoords.push_back(u);
			meshData.texCoords.push_back(v);
		}
	}
}

void TerrainComponent::GenerateNormals()
{
}

void TerrainComponent::GenerateMesh()
{
	if (!baseHeightmap) {
		std::cerr << "Error: Cannot generate mesh - no base heightmap set!" << std::endl;
		return;
	}

	meshData.Clear();

	std::cout << "Generating terrain mesh..." << std::endl;

	GenerateVertices();    // Need vertices first for normals calculation
	GenerateNormals();     // Normals depend on vertex positions
	GenerateTexCoords();   // Texcoords are independent
	GenerateIndices();     // Indices depend on vertex count

	std::cout << "Terrain mesh generated: " << std::endl;
}

void TerrainComponent::GenerateFromHeightmap(const Heightmap* heightmap, float yScale, float yShift)
{
	if (!baseHeightmap) { return; }

	baseHeightmap->SetHeightMapTransform(yScale, yShift);
	GenerateMesh();
}

void TerrainComponent::OnRender(Shader& shader, const glm::mat4& modelMatrix)
{

}

float TerrainComponent::Lerp(float a, float b, float t) {
	return a + t * (b - a);
}

float TerrainComponent::Lerp(float a, float b, float t) const
{
	return a + t * (b - a);
}


float TerrainComponent::SampleHeightmap(Heightmap* heightmap, int x, int y, const TerrainLayer* layer) const
{
	if (!heightmap) return 0.0f;

	const auto& info = heightmap->GetHeightmapSize();

	float sampleX = static_cast<float>(x) / (info.width - 1);
	float sampleY = static_cast<float>(y) / (info.height - 1);

	if (layer) {
		sampleX = sampleX * layer->scale.x + layer->offset.x;
		sampleY = sampleY * layer->scale.y + layer->offset.y;
	}

	//bilinear sampling section

	float texX = sampleX * (info.width - 1);
	float texY = sampleY * (info.height - 1);

	int x1 = static_cast<int>(texX);
	int y1 = static_cast<int>(texY);
	int x2 = (x1 + 1) % info.width;
	int y2 = (y1 + 1) % info.height;

	float fracX = texX - x1;
	float fracY = texY - y1;

	float h11 = static_cast<float>(heightmap->GetPixel(x1, y1)) / 256.0f;
	float h12 = static_cast<float>(heightmap->GetPixel(x1, y2)) / 256.0f;
	float h21 = static_cast<float>(heightmap->GetPixel(x2, y1)) / 256.0f;
	float h22 = static_cast<float>(heightmap->GetPixel(x2, y2)) / 256.0f;

	float top = Lerp(h11, h21, fracX);
	float bot = Lerp(h12, h22, fracX);
	return Lerp(top, bot, fracY);

}


float TerrainComponent::CalculateFinalHeight(int x, int y) const
{
	if (!baseHeightmap) return 0.0f;

	const auto& baseHeightmapInfo = baseHeightmap->GetHeightmapSize();
	float baseNormalised = SampleHeightmap(baseHeightmap.get(), x, y, nullptr);

	for (const auto& layer : layers) {
		if (!layer.heightmap) continue;

		float layerNormalized = SampleHeightmap(layer.heightmap.get(), x, y, &layer);

		switch (layer.blendMode) {
		case(TerrainBlendMode::Add):
			baseNormalised += layerNormalized * layer.weight;
			break;
		case(TerrainBlendMode::Subtract):
			baseNormalised -= layerNormalized * layer.weight;
			break;
		}
	}

	return baseNormalised * baseHeightmapInfo.yScale - baseHeightmapInfo.yShift;
}
