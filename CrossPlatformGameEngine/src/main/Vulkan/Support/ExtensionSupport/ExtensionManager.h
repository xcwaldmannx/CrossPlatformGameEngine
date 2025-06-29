#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <vector>

namespace vulkan {

	class ExtensionManager {
	public:
		ExtensionManager();

		void addExtension(const char* extension);
		std::vector<const char*>& getExtensions();

		void validateSupport();
		bool validateExtensionSupport(const char** validateExtensions, size_t validateExtensionsCount);

	private:
		std::vector<const char*> mAdditionalExtensions;
		std::vector<const char*> mExtensions;
	};

}