#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class WindowManager
{
public:
	WindowManager();

	void init();
	void destroy();

	void pollEvents() const;

	bool isRunning() const;

	GLFWwindow* getWindow() const;

private:
	GLFWwindow* mWindow = nullptr;

	const uint32_t WINDOW_WIDTH = 800;
	const uint32_t WINDOW_HEIGHT = 600;

};