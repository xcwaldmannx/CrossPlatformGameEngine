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

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void iconifyCallback(GLFWwindow* window, int iconified);

	void setResized(bool resized);

	bool isRunning() const;
	bool isResized() const;
	bool isMinimized() const;

	GLFWwindow* getWindow() const;
	InputManager& getInput();

	inline static int getWidth()
	{
		return mFramebufferWidth.load(std::memory_order_acquire);
	}

	inline static int getHeight()
	{
		return mFramebufferHeight.load(std::memory_order_acquire);
	}

private:
	void windowThread();

private:
	GLFWwindow* mWindow = nullptr;

	std::atomic<bool> mIsRunning = false;
	std::atomic<bool> mWindowReady = false;

	inline static std::atomic<int> mFramebufferWidth = 0;
	inline static std::atomic<int> mFramebufferHeight = 0;
	inline static std::atomic<bool> mIsResized = false;
	inline static std::atomic<bool> mIsMinimized = false;

	std::thread mWindowThread;

	InputManager mInputManager;

	const uint32_t WINDOW_WIDTH = 800;
	const uint32_t WINDOW_HEIGHT = 600;

};