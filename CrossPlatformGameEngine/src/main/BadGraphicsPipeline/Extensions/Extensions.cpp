#include "Extensions.h"

#include <iostream>

/*
Begin Extensions Implementation
*/

Extensions::Extensions() {}

void Extensions::add(const char* extension) {
    mRequiredExtensions.push_back(extension);
}

void Extensions::add(const char** extensions, size_t count) {
    mRequiredExtensions.insert(mRequiredExtensions.end(), extensions, extensions + count);
}

void Extensions::add(std::vector<const char*> extensions) {
    mRequiredExtensions.insert(mRequiredExtensions.end(), extensions.begin(), extensions.end());
}

void Extensions::validate() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    bool failed = false;

    for (const auto& required : mRequiredExtensions) {
        bool found = false;

        for (const auto& available : availableExtensions) {
            if (strcmp(available.extensionName, required) == 0) {
                std::cout << "Found extension: " << available.extensionName << '\n';
                found = true;
                break;
            }
        }

        if (!found) {
            std::cout << "Extension not found: " << required << '\n';
            failed = true;
        }
    }

    if (failed) {
        throw std::runtime_error("There are missing extensions.");
    }
}

std::vector<const char*> Extensions::get() const {
    return mRequiredExtensions;
}

/*
End Extensions Implementation
*/
