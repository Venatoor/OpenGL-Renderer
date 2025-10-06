#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

struct HeightmapInfo {
	int width;
	int height;
	int nChannels;
	float yScale;
	float yShift;
};


class Heightmap
{
public:

	Heightmap(int width, int height, unsigned char fillValue);
	Heightmap(const std::string& filename, bool flipVerticallyOnLoad);

	~Heightmap();

	
	const HeightmapInfo& GetHeightmapSize() { return heightmapSize; }
	void SetHeightMapTransform(float yScale, float yShift);
	void CreateEmpty(int width, int height, unsigned char fillValue);
	void ReleaseData();

	//=== some getters and setters if i might want individual pixels ( or texels ? ) treatment 
	void SetupHeightmapData(float yScale, float yShift);
	unsigned char* GetData() const { return data; }


	unsigned char GetPixel(int x, int y) const;
	void SetPixel(int x, int y, unsigned char value);



private:

	bool LoadFromFile(const std::string& filename, bool flipVerticallyOnLoad);
	unsigned char* data;
	HeightmapInfo heightmapSize;

	
};

