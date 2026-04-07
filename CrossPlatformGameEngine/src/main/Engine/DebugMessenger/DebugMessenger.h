#pragma once

#include <vulkan/vulkan.h>

namespace ascen
{

	class DebugMessenger
	{
	public:
		static VkDebugUtilsMessengerEXT create(VkInstance vkInstance);
		static void destroy(VkInstance vkInstance, VkDebugUtilsMessengerEXT messenger);

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT severity,
			VkDebugUtilsMessageTypeFlagsEXT type,
			const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
			void* userData);
	};

}
