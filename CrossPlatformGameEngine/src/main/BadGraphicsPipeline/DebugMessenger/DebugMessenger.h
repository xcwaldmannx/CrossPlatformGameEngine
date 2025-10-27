#pragma once

#include <vulkan/vulkan.h>

/*
FILE: DebugMessenger.h
NAME: DebugMessenger
TYPE: CLASS
DESC: Vulkan Debug Messenger management
*/
class DebugMessenger {
public:
	void create(VkInstance vkInstance);
	void destroy(VkInstance vkInstance);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData);

private:
	VkDebugUtilsMessengerEXT mVkDebugMessenger = nullptr;
};