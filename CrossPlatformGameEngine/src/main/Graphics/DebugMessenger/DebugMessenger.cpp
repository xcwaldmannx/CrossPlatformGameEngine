#include "DebugMessenger.h"

#include <stdexcept>
#include <iostream>

using namespace ascen;

VkDebugUtilsMessengerEXT DebugMessenger::create(VkInstance vkInstance)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");

    VkDebugUtilsMessengerEXT messenger{};

    if (func && func(vkInstance, &createInfo, nullptr, &messenger) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create debug messenger.");
    }

    return messenger;
}

void DebugMessenger::destroy(VkInstance vkInstance, VkDebugUtilsMessengerEXT messenger)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (func)
    {
        func(vkInstance, messenger, nullptr);
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData)
{

    std::cout << "Validation Layer: " << callbackData->pMessage << std::endl;
    return VK_FALSE;
}
