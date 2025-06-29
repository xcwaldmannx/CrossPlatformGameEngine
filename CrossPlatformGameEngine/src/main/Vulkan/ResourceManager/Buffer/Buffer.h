#pragma once

#include <vector>

#include <Vulkan/vulkan.h>

namespace vulkan {

	class Buffer {
	public:
		const VkBuffer& getBuffer() const;
		const size_t size() const;
	private:
		VkBuffer mBuffer = nullptr;
		VkDeviceMemory mMemory = nullptr;

		size_t mSize;

		friend class ResourceManager;
		friend class MemoryAllocator;
	};

}
