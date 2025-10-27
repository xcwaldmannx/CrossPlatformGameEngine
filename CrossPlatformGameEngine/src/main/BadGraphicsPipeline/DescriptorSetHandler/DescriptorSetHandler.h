#pragma once

#include "../LogicalDeviceHandler/LogicalDeviceHandler.h"

#include "../Resource/ResourceCommonTypes.h"

#include <vector>

#include <vulkan/vulkan.h>


namespace ascen {

	struct Buffer;
	struct Texture;

	struct DescriptorGroupCreateInfo {
		LogicalDevice* logicalDevice;
		const std::vector<VkDescriptorSetLayoutBinding>* bindings;
		Buffer* mUniformBuffer;
		Buffer* mStorageBuffer;
		TextureArray* mTextureArray;
	};

	struct DescriptorGroup {
		VkDescriptorPool mPool = nullptr;
		VkDescriptorSetLayout mLayout = nullptr;
		std::vector<VkDescriptorSet> mSets;
	};

	struct DescriptorSetLayout {
		VkDescriptorSetLayout mLayout = nullptr;
	};

	struct DescriptorPool {
		VkDescriptorPool mPool;
	};

	enum DescriptorType {
		UNIFORM_BUFFER         = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		UNIFORM_BUFFER_DYNAMIC = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		STORAGE_BUFFER         = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		STORAGE_BUFFER_DYNAMIC = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
		TEXTURE_SAMPLER        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	};

	enum DescriptorStageFlags {
		VERTEX_SHADER = VK_SHADER_STAGE_VERTEX_BIT,
		PIXEL_SHADER = VK_SHADER_STAGE_FRAGMENT_BIT,
		COMPUTE_SHADER = VK_SHADER_STAGE_COMPUTE_BIT,
	};

	/*
	FILE: DescriptorSetHandler.h
	NAME: createDescriptorGroup
	RTRN: void
	DESC: Handles Descriptor Group creation.
	*/
	void createDescriptorGroup(
		const DescriptorGroupCreateInfo& info,
		DescriptorGroup* descriptorGroup);

	/*
	FILE: DescriptorSetHandler.h
	NAME: createDescriptorPool
	RTRN: void
	DESC: Handles Descriptor Pool creation.
	*/
	void createDescriptorPool(
		LogicalDevice& logicalDevice,
		DescriptorGroup* descriptorGroup);

	/*
	FILE: DescriptorSetHandler.h
	NAME: createDescriptorSetLayout
	RTRN: void
	DESC: Handles Descriptor Set Layout creation.
	*/
	void createDescriptorSetLayout(
		LogicalDevice& logicalDevice,
		const std::vector<VkDescriptorSetLayoutBinding>& bindings,
		DescriptorGroup* descriptorGroup);

	/*
	FILE: DescriptorSetHandler.h
	NAME: createDescriptorSetLayoutBinding
	RTRN: void
	DESC: Handles Descriptor Set Layout Binding creation. There is no related call to destroy, as it is not necessary.
	*/
	void createDescriptorSetLayoutBinding(
		uint32_t slot,
		DescriptorType type,
		unsigned int flags,
		VkDescriptorSetLayoutBinding* binding);

	void createDescriptorSet(
		LogicalDevice& logicalDevice,
		Buffer& uniformBuffer,
		Buffer& storageBuffer,
		TextureArray& textureArray,
		DescriptorGroup* descriptorGroup);

	/*
	FILE: DescriptorSetHandler.h
	NAME: createDescriptorSets
	RTRN: void
	DESC: Handles Descriptor Set creation.
	*/
	void createDescriptorSets(
		LogicalDevice& logicalDevice,
		const std::vector<std::vector<Buffer>>& buffers,
		const std::vector<Texture>& textures,
		uint32_t maxFramesInFlight,
		DescriptorGroup* descriptorGroup);

	/*
	FILE: DescriptorSetHandler.h
	NAME: destroyDescriptorGroup
	RTRN: void
	DESC: Handles Descriptor Group destruction.
	*/
	void destroyDescriptorGroup(LogicalDevice& logicalDevice, DescriptorGroup& descriptorGroup);

}