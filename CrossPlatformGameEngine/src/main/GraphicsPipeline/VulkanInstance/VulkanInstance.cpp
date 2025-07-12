#include "VulkanInstance.h"

#include <stdexcept>

namespace ascen {

    void createVulkanInstance(VulkanInstance* instance) {
        // Enable Validation Layers
        if (instance->mValidationLayers.isEnabled()) {
            instance->mValidationLayers.validate();

            // Add Debug extension if validation layers are enabled
            instance->mExtensions.add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        // Add and validate Extensions
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        instance->mExtensions.add(glfwExtensions, glfwExtensionCount);
        instance->mExtensions.validate();

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "My Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        auto extensions = instance->mExtensions.get();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (instance->mValidationLayers.isEnabled()) {
            const auto& validationLayers = instance->mValidationLayers.get();
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
            debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugCreateInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugCreateInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugCreateInfo.pfnUserCallback = &instance->mDebugMessenger.debugCallback;
        }
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance->mInstance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create VK instance!");
        }

        // Vulkan instance exists, so now we can create the DebugMessenger
        instance->mDebugMessenger.create(instance->mInstance);
    }

    void destroyVulkanInstance(VulkanInstance& instance) {
        instance.mDebugMessenger.destroy(instance.mInstance);
        vkDestroyInstance(instance.mInstance, nullptr);
    }

}
