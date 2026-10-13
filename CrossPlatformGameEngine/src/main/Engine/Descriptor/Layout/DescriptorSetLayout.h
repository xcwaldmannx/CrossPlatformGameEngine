#pragma once

#include "../../Handle/Handle.h"
#include "../../Core/Types.h"

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
			VkDevice device,
			const std::vector<Binding>& bindings);

	public:
		void destroy(VkDevice device) override;

	private:
		friend class DescriptorFactory;
	};

}
