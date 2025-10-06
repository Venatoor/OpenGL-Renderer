#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class Viewport
{
public:

	Viewport() = default;
	Viewport(int x, int y, int width, int height);

	void Bind() const;
	void Resize(int newWidth, int newHeight);
	void SetPosition(int newX, int newY);

	glm::ivec2 GetPosition() const { return {x,y}; }
	glm::ivec2 GetSize() const { return { width, height }; }
	float GetAspectRatio() { return static_cast<float>(width) / height; }

	static void RestoreDefault();

private:

	int x = 0;
	int y = 0;

	int height = 0;
	int width = 0;

};

