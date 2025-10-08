#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	static const bool ENABLE_VALIDATION_LAYERS;

	class ValidationLayers
	{
	public:
		static void add(const char* layer, std::vector<const char*>* layerList);
		static void add(const char** layers, size_t count, std::vector<const char*>* layerList);
		static void add(std::vector<const char*> layers, std::vector<const char*>* layerList);

		static void validate(const std::vector<const char*>& layerList);

		static bool isEnabled();

	};

}
