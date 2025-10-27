#include "Extensions.h"

#include <iostream>

using namespace ascen;

void Extensions::add(const char* extension, std::vector<const char*>* extensionList)
{
    extensionList->push_back(extension);
}

void Extensions::add(const char** extensions, size_t count, std::vector<const char*>* extensionList)
{
    extensionList->insert(extensionList->end(), extensions, extensions + count);
}

void Extensions::add(std::vector<const char*> extensions, std::vector<const char*>* extensionList)
{
    extensionList->insert(extensionList->end(), extensions.begin(), extensions.end());
}

void Extensions::validate(const std::vector<const char*>& extensionList)
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    bool failed = false;

    for (const auto& required : extensionList)
    {
        bool found = false;

        for (const auto& available : availableExtensions)
        {
            if (strcmp(available.extensionName, required) == 0) {
                std::cout << "Found extension: " << available.extensionName << '\n';
                found = true;
                break;
            }
        }

        if (!found)
        {
            std::cout << "Extension not found: " << required << '\n';
            failed = true;
        }
    }

    if (failed)
    {
        throw std::runtime_error("There are missing extensions.");
    }
}
