#pragma once

#include <vector>

#include <vulkan/vulkan.h>

/*
FILE: Extensions.h
NAME: Extensions
TYPE: CLASS
DESC: Vulkan Extensions management
*/
class Extensions {
public:
	Extensions();

	void add(const char* extension);
	void add(const char** extensions, size_t count);
	void add(std::vector<const char*> extensions);

	void validate();

	std::vector<const char*> get() const;

private:
	std::vector<const char*> mRequiredExtensions;

};