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
	static void windowCloseCallback(GLFWwindow* window);

	void setResized(bool resized);

	bool isRunning() const;
	bool isResized() const;
	bool isMinimized() const;
	bool isCloseRequested() const;

	GLFWwindow* getWindow() const;
	InputManager& getInput();

	static int getWidth()
	{
		return mFramebufferWidth.load(std::memory_order_acquire);
	}

	static int getHeight()
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
	inline static std::atomic<bool> mIsCloseRequested = false;

	std::thread mWindowThread;

	InputManager mInputManager;

	bool mFocusMouse = false;
	const uint32_t WINDOW_WIDTH = 800;
	const uint32_t WINDOW_HEIGHT = 600;

};