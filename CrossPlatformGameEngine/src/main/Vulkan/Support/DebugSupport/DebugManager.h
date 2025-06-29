#pragma once

#include <vulkan/vulkan.h>

namespace vulkan {
	class DebugManager {
	public:
        VkResult create(VkInstance instance);
        void destroy(VkInstance instance);

        void setCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);

    private:
        VkDebugUtilsMessengerEXT mVkDebugMessenger = nullptr;
	};
}