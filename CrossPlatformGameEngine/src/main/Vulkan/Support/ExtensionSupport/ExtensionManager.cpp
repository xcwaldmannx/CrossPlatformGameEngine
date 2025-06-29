#include "ExtensionManager.h"

#include <iostream>
#include <cstring>

using namespace vulkan;

ExtensionManager::ExtensionManager() {
    mAdditionalExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}

void ExtensionManager::addExtension(const char* extension) {
    mAdditionalExtensions.push_back(extension);
}

std::vector<const char*>& ExtensionManager::getExtensions() {
    return mExtensions;
}

void ExtensionManager::validateSupport() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    mExtensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
    mExtensions.insert(mExtensions.end(), mAdditionalExtensions.begin(), mAdditionalExtensions.end());

    if (!validateExtensionSupport(mExtensions.data(), mExtensions.size())) {
        throw std::runtime_error("Extensions are missing!");
    }
}

bool ExtensionManager::validateExtensionSupport(const char** validateExtensions, size_t validateExtensionsCount) {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    bool failed = false;

    for (int i = 0; i < validateExtensionsCount; i++) {
        bool found = false;

        for (const auto& extension : extensions) {
            if (strcmp(extension.extensionName, validateExtensions[i]) == 0) {
                std::cout << "Found extension: " << extension.extensionName << '\n';
                found = true;
                break;
            }
        }

        if (!found) {
            std::cout << "Extension not found: " << validateExtensions[i] << '\n';
            failed = true;
        }
    }

    return !failed;
}
