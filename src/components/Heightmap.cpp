#include "Heightmap.h"
#include <stb/stb_image.h>
#include <iostream>
#include <algorithm>
#include <cmath>

Heightmap::Heightmap(int width, int height, unsigned char fillValue)
{
	CreateEmpty( width,  height,  fillValue);
}

Heightmap::Heightmap(const std::string& filename, bool flipVerticallyOnLoad)
{
	LoadFromFile(filename, flipVerticallyOnLoad);
}

Heightmap::~Heightmap()
{
	ReleaseData();
}

bool Heightmap::LoadFromFile(const std::string& filename, bool flipVerticallyOnLoad)
{
	ReleaseData();


	if (flipVerticallyOnLoad) stbi_set_flip_vertically_on_load(true);

	data = stbi_load(filename.c_str(),
					&heightmapSize.width, 
				    &heightmapSize.height, 
		            &heightmapSize.nChannels, 
		            0);

	if (flipVerticallyOnLoad) stbi_set_flip_vertically_on_load(false);

	if (!data) {
		std::cerr << "Failed to load heightmap" << filename << std::endl;
		return false;
	}

	std::cout << "Loaded heightmap with : " << filename << " Width : " << heightmapSize.width << " Height : " << heightmapSize.height
		<< " Channels : " << heightmapSize.nChannels << std::endl;

	return true;
}

void Heightmap::SetHeightMapTransform(float yScale, float yShift)
{
	float normalisedYScale = yScale;
	float normalisedYShift = yShift;
	heightmapSize.yScale = normalisedYScale;
	heightmapSize.yShift = normalisedYShift;
}

void Heightmap::CreateEmpty(int width, int height, unsigned char fillValue)
{
	ReleaseData();

	heightmapSize.nChannels = 1;
	heightmapSize.width = width;
	heightmapSize.height = height;

	data = static_cast<unsigned char*>(malloc(width * height * sizeof(unsigned char)));
	if (data) {
		std::fill(data, data + (width * height), fillValue);
	}
}

void Heightmap::ReleaseData()
{
	if (data) {
		stbi_image_free(data);
		data = nullptr;
	}
	heightmapSize.nChannels = 0;
	heightmapSize.height = 0;
	heightmapSize.width = 0;
}

void Heightmap::SetupHeightmapData(float yScale, float yShift)
{
	SetHeightMapTransform(yScale, yShift);
}

/*
void Heightmap::GenerateVertices()
{
	if (!data) {
		std::cerr << "Can't find any empty data or image !" << std::endl;
		return;
	}

	vertices.clear();
	vertices.reserve(heightmapSize.width * heightmapSize.height * 3);

	for (unsigned int i = 0; i < heightmapSize.height; i++) {
		for (unsigned int j = 0; j < heightmapSize.width; j++) {
			unsigned char* texel = data + (j + heightmapSize.width * i) * heightmapSize.nChannels;
			unsigned char y = texel[0];

			vertices.push_back(-(heightmapSize.height / 2.0f) + i); // filling x 
			vertices.push_back((int)y * heightmapSize.yScale - heightmapSize.yShift); // filling with y 
			vertices.push_back(-(heightmapSize.width / 2.0f) + j);

		}
	}
}
*/


unsigned char Heightmap::GetPixel(int x, int y) const
{
	if (!data || x < 0 || x >= heightmapSize.width || y < 0 || y >= heightmapSize.height) {
		return 0;
	}

	unsigned char* texel = data + (x + heightmapSize.width * y) * heightmapSize.nChannels;
	return texel[0];
}

void Heightmap::SetPixel(int x, int y, unsigned char value)
{
	if (!data || x < 0 || x >= heightmapSize.width || y < 0 || y >= heightmapSize.height) {
		return;
	}

	unsigned char* texel = data + (x + heightmapSize.width * y) * heightmapSize.nChannels;
	texel[0] = value;
}
