#include "ValidationLayers.h"

#include <stdexcept>

using namespace ascen;

void ValidationLayers::add(const char* layer, std::vector<const char*>* layerList) {
    layerList->push_back(layer);
}

void ValidationLayers::add(const char** layers, size_t count, std::vector<const char*>* layerList) {
    layerList->insert(layerList->end(), layers, layers + count);
}

void ValidationLayers::add(std::vector<const char*> layers, std::vector<const char*>* layerList) {
    layerList->insert(layerList->end(), layers.begin(), layers.end());
}

void ValidationLayers::validate(const std::vector<const char*>& layerList)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : layerList)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }
    }
}

bool ValidationLayers::isEnabled()
{
    return ENABLE_VALIDATION_LAYERS;
}
