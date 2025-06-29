#pragma once

#include <vector>

#include <vulkan/vulkan.h>

/*
FILE: ValidationLayers.h
NAME: ValidationLayers
TYPE: CLASS
DESC: Vulkan Validation Layer management
*/
class ValidationLayers {
public:
	void validate();
	const std::vector<const char*>& get() const;
	bool isEnabled() const;

private:
	const std::vector<const char*> VALIDATION_LAYERS = {
	"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool ENABLE_VALIDATION_LAYERS = false;
#else
	const bool ENABLE_VALIDATION_LAYERS = true;
#endif
};
