#pragma once

#include "../../Handle/Handle.h"
#include "../../Types.h"
#include "../Pool/DescriptorPool.h"
#include "../Layout/DescriptorSetLayout.h"

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
			std::variant<VkDescriptorBufferInfo, VkDescriptorImageInfo> mInfo{};
		};

	private:
		DescriptorSet(
			VkDevice device,
			const DescriptorPoolPtr& pool,
			const DescriptorSetLayoutPtr& layout,
			std::vector<Write> writes);

	public:
		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

	private:
		friend class DescriptorFactory;
	};

}
