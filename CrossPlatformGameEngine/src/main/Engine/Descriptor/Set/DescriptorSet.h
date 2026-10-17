#pragma once

#include "../../Handle/Handle.h"
#include "../../Core/Types.h"

#include <variant>

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class DescriptorSet : public Handle<VkDescriptorSet>
	{
	public:
		struct Write
		{
			VkWriteDescriptorSet mWrite{};
			std::variant<
				VkDescriptorBufferInfo,
				VkDescriptorImageInfo,
				std::vector<VkDescriptorBufferInfo>,
				std::vector<VkDescriptorImageInfo>> mInfo{};
		};

		void updateBuffer(
			VkDevice device,
			const uint32_t binding,
			const VkDescriptorType type,
			VkBuffer buffer,
			const VkDeviceSize range) const;

		void updateTexture(
			VkDevice device,
			uint32_t binding,
			VkDescriptorType type,
			VkImageView imageView) const;

		void updateTextures(
			VkDevice device,
			const uint32_t binding,
			const VkDescriptorType type,
			const std::vector<VkImageView>& imageViews) const;

	private:
		DescriptorSet(
			VkDevice device,
			const DescriptorPoolPtr& pool,
			const DescriptorSetLayoutPtr& layout,
			std::vector<Write> writes);

	public:
		void destroy(VkDevice device) override;

	private:
		VkDescriptorPool mPool = VK_NULL_HANDLE;

		friend class DescriptorFactory;
	};

}
