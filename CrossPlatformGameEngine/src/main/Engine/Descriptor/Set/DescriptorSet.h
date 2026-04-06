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
			std::variant<VkDescriptorBufferInfo, VkDescriptorImageInfo> mInfo{};
		};

	private:
		DescriptorSet(
			VkDevice device,
			const DescriptorPoolPtr& pool,
			const DescriptorSetLayoutPtr& layout,
			std::vector<Write> writes);

	public:
		void destroy(VkDevice device) override;

	private:
		friend class DescriptorFactory;
	};

}
