#include "ValidationLayerManager.h"

#include <stdexcept>
#include <cstring>

using namespace vulkan;

ValidationLayerManager::ValidationLayerManager() {}

const std::vector<const char*>& ValidationLayerManager::getValidationLayers() {
    return mValidationLayers;
}

void ValidationLayerManager::validateSupport() const {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : mValidationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
    }
}

bool ValidationLayerManager::isEnabled() const {
    return mEnableValidationLayers;
}
