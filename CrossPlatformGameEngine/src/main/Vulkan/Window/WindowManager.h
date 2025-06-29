#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

namespace vulkan {

	class WindowManager {
	public:
		void create();
		void destroy();

		GLFWwindow* getWindow() const;

		bool isResized();
	private:

		static inline void frameBufferResizeCallback(GLFWwindow* window, int width, int height);

	private:

		GLFWwindow* mGlfwWindow = nullptr;

		const uint32_t WINDOW_WIDTH = 800;
		const uint32_t WINDOW_HEIGHT = 600;

		bool mIsResized = false;
	};

}