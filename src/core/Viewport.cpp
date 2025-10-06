#include "Viewport.h"

Viewport::Viewport(int x, int y, int width, int height) :
	x(x),
	y(y),
	width(width),
	height(height)
{
	Bind();
}

void Viewport::Bind() const
{
	glViewport(x, y, width, height);
}

void Viewport::Resize(int newWidth, int newHeight)
{
	width = newWidth;
	height = newHeight;
}

void Viewport::SetPosition(int newX, int newY)
{
	x = newX;
	y = newY;
}

void Viewport::RestoreDefault()
{
	GLint dims[4] = { 0 };
	glGetIntegerv(GL_VIEWPORT, dims);
	glViewport(0, 0, dims[2], dims[3]);
}
