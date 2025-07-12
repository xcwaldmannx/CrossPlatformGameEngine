#include "DescriptorSetHandler.h"

#include <stdexcept>

#include <array>

/*
Begin DescriptorSetHandler Implementation
*/

namespace ascen {

	void createDescriptorGroup(
		const DescriptorGroupCreateInfo& info,
		DescriptorGroup* descriptorGroup)
	{
		createDescriptorPool(*info.logicalDevice, descriptorGroup);
		createDescriptorSetLayout(*info.logicalDevice, *info.bindings, descriptorGroup);
		createDescriptorSet(
			*info.logicalDevice,
			*info.mUniformBuffer,
			*info.mStorageBuffer,
			*info.mTextureArray,
			descriptorGroup);
	}

	void createDescriptorPool(
		LogicalDevice& logicalDevice,
		DescriptorGroup* descriptorGroup)
	{
		std::array<VkDescriptorPoolSize, 3> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes[0].descriptorCount = 1;

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		poolSizes[1].descriptorCount = 1;

		poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[2].descriptorCount = 1;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1;

		if (vkCreateDescriptorPool(
			logicalDevice.mDevice, &poolInfo, nullptr, &descriptorGroup->mPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void createDescriptorSetLayout(
		LogicalDevice& logicalDevice,
		const std::vector<VkDescriptorSetLayoutBinding>& bindings,
		DescriptorGroup* descriptorGroup)
	{

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = (uint32_t) bindings.size();
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(
			logicalDevice.mDevice, &layoutInfo, nullptr, &descriptorGroup->mLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void createDescriptorSet(
		LogicalDevice& logicalDevice,
		Buffer& uniformBuffer,
		Buffer& storageBuffer,
		TextureArray& textureArray,
		DescriptorGroup* descriptorGroup)
	{
		descriptorGroup->mSets.resize(1);

		std::vector<VkDescriptorSetLayout> layouts(1, descriptorGroup->mLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorGroup->mPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts.data();

		if (vkAllocateDescriptorSets(
			logicalDevice.mDevice, &allocInfo, descriptorGroup->mSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffer.mBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = uniformBuffer.mSizeBytes;

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureArray.mView;
		imageInfo.sampler = textureArray.mSampler;

		VkDescriptorBufferInfo storageBufferInfo{};
		storageBufferInfo.buffer = storageBuffer.mBuffer;
		storageBufferInfo.offset = 0;
		storageBufferInfo.range = storageBuffer.mBufferSizeBytes;

		std::vector<VkWriteDescriptorSet> descriptorWrites(3);

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorGroup->mSets[0];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorGroup->mSets[0];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &storageBufferInfo;

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstSet = descriptorGroup->mSets[0];
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(
			logicalDevice.mDevice,
			descriptorWrites.size(),
			descriptorWrites.data(),
			0,
			nullptr);

	}

	// TODO: Refactor how createDescriptorSets works
	void createDescriptorSets(
		LogicalDevice& logicalDevice,
		const std::vector<std::vector<Buffer>>& buffers,
		const std::vector<Texture>& textures,
		uint32_t maxFramesInFlight,
		DescriptorGroup* descriptorGroup) {

		descriptorGroup->mSets.resize(maxFramesInFlight);

		std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight, descriptorGroup->mLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorGroup->mPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFramesInFlight);
		allocInfo.pSetLayouts = layouts.data();

		if (vkAllocateDescriptorSets(
			logicalDevice.mDevice, &allocInfo, descriptorGroup->mSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		std::vector<VkDescriptorBufferInfo> bufferInfos;
		bufferInfos.resize(buffers.size() * maxFramesInFlight);

		std::vector<VkDescriptorImageInfo> imageInfos;
		imageInfos.resize(textures.size() * maxFramesInFlight);

		std::vector<VkWriteDescriptorSet> descriptorWrites;
		descriptorWrites.resize((buffers.size() + textures.size()) * maxFramesInFlight);

		size_t descriptorWriteIndex = 0;

		for (size_t frame = 0; frame < maxFramesInFlight; frame++) {
			// --- Buffers for this frame ---
			for (size_t bufIdx = 0; bufIdx < buffers.size(); bufIdx++) {
				const Buffer& buffer = buffers[bufIdx][frame];
				size_t infoIdx = frame * buffers.size() + bufIdx;
				size_t writeIdx = descriptorWriteIndex++;

				bufferInfos[infoIdx].buffer = buffer.mBuffer;
				bufferInfos[infoIdx].offset = 0;
				bufferInfos[infoIdx].range = buffer.mSizeBytes;

				descriptorWrites[writeIdx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[writeIdx].dstSet = descriptorGroup->mSets[frame];
				descriptorWrites[writeIdx].dstBinding = 0;
				descriptorWrites[writeIdx].dstArrayElement = 0;
				descriptorWrites[writeIdx].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[writeIdx].descriptorCount = 1;
				descriptorWrites[writeIdx].pBufferInfo = &bufferInfos[infoIdx];
			}

			// --- Textures for this frame ---
			for (size_t texIdx = 0; texIdx < textures.size(); texIdx++) {
				const Texture& texture = textures[texIdx];
				size_t infoIdx = frame * textures.size() + texIdx;
				size_t writeIdx = descriptorWriteIndex++;

				imageInfos[infoIdx].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfos[infoIdx].imageView = texture.mView;
				imageInfos[infoIdx].sampler = texture.mSampler;

				descriptorWrites[writeIdx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[writeIdx].dstSet = descriptorGroup->mSets[frame];
				descriptorWrites[writeIdx].dstBinding = 1;
				descriptorWrites[writeIdx].dstArrayElement = 0;
				descriptorWrites[writeIdx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[writeIdx].descriptorCount = 1;
				descriptorWrites[writeIdx].pImageInfo = &imageInfos[infoIdx];
			}
		}


		vkUpdateDescriptorSets(
			logicalDevice.mDevice, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
	}

	void createDescriptorSetLayoutBinding(
		uint32_t slot,
		DescriptorType type,
		unsigned int flags,
		VkDescriptorSetLayoutBinding* binding) {

		binding->binding = slot;
		binding->descriptorType = (VkDescriptorType) type;
		binding->descriptorCount = 1;
		binding->stageFlags = (VkShaderStageFlagBits) flags;
		binding->pImmutableSamplers = nullptr;
	}

	void destroyDescriptorGroup(LogicalDevice& logicalDevice, DescriptorGroup& descriptorGroup) {
		vkDestroyDescriptorSetLayout(logicalDevice.mDevice, descriptorGroup.mLayout, nullptr);
		vkDestroyDescriptorPool(logicalDevice.mDevice, descriptorGroup.mPool, nullptr);
	}
}

/*
End DescriptorSetHandler Implementation
*/
