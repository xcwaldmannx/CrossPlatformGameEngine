#pragma once

#include "../../HandleManager/Handle.h"
#include "../../Types.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class DescriptorSetLayout : public Handle<VkDescriptorSetLayout>
	{
	public:
		using Binding = VkDescriptorSetLayoutBinding;

	private:
		DescriptorSetLayout(
			const std::vector<Binding>& bindings);

	public:
		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

	private:
		VkDescriptorSetLayoutCreateInfo mCreateInfo{};

		friend class DescriptorFactory;
	};

}
