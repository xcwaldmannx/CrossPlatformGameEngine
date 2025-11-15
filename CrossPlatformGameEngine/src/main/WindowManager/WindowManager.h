#pragma once

#include "InputManager/InputManager.h"

#include <atomic>
#include <thread>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class WindowManager
{
public:
	void create();
	void destroy();

	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	void setResized(bool resized);

	bool isRunning() const;
	bool isResized() const;

	GLFWwindow* getWindow() const;
	InputManager& getInput();

private:
	void windowThread();

private:
	GLFWwindow* mWindow = nullptr;

	std::atomic<bool> mIsRunning = false;
	std::atomic<bool> mWindowReady = false;
	std::atomic<bool> mIsResized = false;

	std::thread mWindowThread;

	InputManager mInputManager;

	const uint32_t WINDOW_WIDTH = 800;
	const uint32_t WINDOW_HEIGHT = 600;

};