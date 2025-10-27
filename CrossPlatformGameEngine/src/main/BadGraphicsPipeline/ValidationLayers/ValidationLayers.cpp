#include "ValidationLayers.h"

#include <stdexcept>

/*
Begin ValidationLayers Implementation
*/

void ValidationLayers::validate() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : VALIDATION_LAYERS) {
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

const std::vector<const char*>& ValidationLayers::get() const {
    return VALIDATION_LAYERS;
}

bool ValidationLayers::isEnabled() const {
    return ENABLE_VALIDATION_LAYERS;
}

/*
End ValidationLayers Implementation
*/
