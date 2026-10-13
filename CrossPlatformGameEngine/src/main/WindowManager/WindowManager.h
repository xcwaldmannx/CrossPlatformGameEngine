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

	static void poll();

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void iconifyCallback(GLFWwindow* window, int iconified);
	static void windowCloseCallback(GLFWwindow* window);

	static void setResized(bool resized);

	static bool isRunning();

	static bool isResized();
	static bool isMinimized() ;
	static bool isCloseRequested() ;
	static bool isMouseFocused() ;

	GLFWwindow* getWindow() const;
	InputManager& getInput();

	static uint32_t getWidth()
	{
		return mFramebufferWidth.load(std::memory_order_acquire);
	}

	static uint32_t getHeight()
	{
		return mFramebufferHeight.load(std::memory_order_acquire);
	}

	static uint32_t getCenterX()
	{
		return mFramebufferWidth.load(std::memory_order_acquire) / 2u;
	}

	static uint32_t getCenterY()
	{
		return mFramebufferHeight.load(std::memory_order_acquire) / 2u;
	}

private:
	void windowThread();

private:
	GLFWwindow* mWindow = nullptr;

	inline static std::atomic<bool> mPoll = false;
	inline static std::atomic<bool> mIsRunning = false;
	inline static std::atomic<bool> mWindowReady = false;

	inline static std::atomic<uint32_t> mFramebufferWidth = 0;
	inline static std::atomic<uint32_t> mFramebufferHeight = 0;
	inline static std::atomic<bool> mIsResized = false;
	inline static std::atomic<bool> mIsMinimized = false;
	inline static std::atomic<bool> mIsCloseRequested = false;
	inline static std::atomic<bool> mFocusMouse = false;

	std::thread mWindowThread;

	InputManager mInputManager;

	const uint32_t WINDOW_WIDTH = 800;
	const uint32_t WINDOW_HEIGHT = 600;

};