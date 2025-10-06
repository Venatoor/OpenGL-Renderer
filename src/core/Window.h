#pragma once

typedef struct GLFWwindow GLFWwindow;

class Window
{
public:
	bool Initialize(int width, int height, const char* title);
	void Shutdown();

	bool ShouldClose();
	void PollEvents();
	void SwapBuffers();

	GLFWwindow* GetNativeWindow() const { return window; }

private:

	GLFWwindow* window;
};

