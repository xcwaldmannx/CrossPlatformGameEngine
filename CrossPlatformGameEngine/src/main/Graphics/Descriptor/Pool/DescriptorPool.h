#pragma once

#include "../../Handle/Handle.h"

#include <vector>

namespace ascen
{

	class DescriptorPool : public Handle<VkDescriptorPool>
	{
	public:
		typedef VkDescriptorPoolSize Size;

	private:
		DescriptorPool(VkDevice device, const std::vector<Size>& sizes);

	public:
		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

	private:
		friend class DescriptorFactory;
	};

}
