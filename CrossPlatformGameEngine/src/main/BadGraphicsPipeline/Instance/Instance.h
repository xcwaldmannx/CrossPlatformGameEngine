#pragma once

#include <array>

#include <Vulkan/vulkan.h>
#include <glm/glm.hpp>

struct Instance {
	uint32_t mId;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding = 1;
		bindingDescription.stride = sizeof(Instance);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

		attributeDescriptions[0].binding = 1;
		attributeDescriptions[0].location = 3;
		attributeDescriptions[0].format = VK_FORMAT_R32_UINT;
		attributeDescriptions[0].offset = offsetof(Instance, mId);

			return attributeDescriptions;
	}
};