#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{


	class Extensions
	{
	public:
		static void add(const char* extension, std::vector<const char*>* extensionList);
		static void add(const char** extensions, size_t count, std::vector<const char*>* extensionList);
		static void add(std::vector<const char*> extensions, std::vector<const char*>* extensionList);

		static void validate(const std::vector<const char*>& extensionList);
	};

}
