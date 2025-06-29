#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace vulkan {

	class ValidationLayerManager {
	public:
		ValidationLayerManager();

		const std::vector<const char*>& getValidationLayers();

		void validateSupport() const;
		bool isEnabled() const;

	private:
		const std::vector<const char*> mValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

#ifdef NDEBUG
		const bool mEnableValidationLayers = false;
#else
		const bool mEnableValidationLayers = true;
#endif
	};

}