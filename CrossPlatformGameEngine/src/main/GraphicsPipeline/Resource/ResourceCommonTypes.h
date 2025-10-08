#pragma once

#include <vulkan/vulkan.h>

namespace ascen {

	struct PhysicalDevice;
	struct LogicalDevice;
	struct CommandPool;
	struct Swapchain;

	// IMAGE

	struct ImageCreateInfo {
		PhysicalDevice* mPhysicalDevice;
		LogicalDevice* mLogicalDevice;
		CommandPool* mCommandPool;
		VkQueue mGraphicsQueue;
		VkBufferUsageFlags mUsage;
		VkMemoryPropertyFlags mProperties;
		VkFormat mFormat;
		VkImageTiling mTiling;
		VkImageAspectFlags mFlags;
	};

	struct DepthImageCreateInfo {
		const ImageCreateInfo* mImageCreateInfo;
		const VkExtent2D* mExtent;
	};

	struct Image {
		VkImage mImage;
		VkDeviceMemory mMemory;
		size_t mWidth;
		size_t mHeight;
		size_t mLayers;
	};


	// BUFFER


	enum ResourceBufferType {
		RESRC_VERTEX_BUFFER = 0x00,
		RESRC_UNIFORM_BUFFER = 0x01,
		RESRC_XXXX_BUFFER = 0x02,
		RESRC_XXYY_BUFFER = 0x03,
	};

	struct BufferCreateInfo {
		PhysicalDevice* mPhysicalDevice;
		LogicalDevice* mLogicalDevice;
		VkQueue mGraphicsQueue;
		CommandPool* mCommandPool;
	};

	struct Buffer {
		VkBuffer mBuffer;
		VkDeviceMemory mMemory;
		void* mMappedMemory;
		size_t mSize;            // number of elements in the buffer
		size_t mSizeBytes;       // size of one element in the buffer in bytes
		size_t mBufferSizeBytes; // total buffer size in bytes
	};


	// TEXTURE


	enum TextureFormat {
		TEX_FORMAT_RGBA8_SRGB = VK_FORMAT_R8G8B8A8_SRGB
	};

	enum TextureTiling {
		TEX_TILING_OPTIMAL = VK_IMAGE_TILING_OPTIMAL
	};

	enum ResourceTextureType {
		RESRC_RGBA8_TEXTURE = 0x01,
		RESRC_RGB8_TEXTURE  = 0x02,
		RESRC_XXXX_TEXTURE  = 0x03,
		RESRC_XXYY_TEXTURE  = 0x04,
	};

	struct TextureCreateInfo {
		ImageCreateInfo* mImageCreateInfo;
		Image mImage;
		VkImageView mView;
		VkSampler mSampler;
	};

	struct DepthTextureCreateInfo {
		DepthImageCreateInfo* mDepthImageCreateInfo;
		Image mImage;
		VkImageView mView;
	};

	struct Texture {
		Image mImage;
		VkImageView mView;
		VkSampler mSampler;
	};


	// TEXTURE ARRAY


	struct TextureArrayCreateInfo {
		ImageCreateInfo* mImageCreateInfo;
		VkSampler mSampler;
	};

	struct TextureArray {
		Image mImage;
		VkImageView mView;
		VkSampler mSampler;
	};

}