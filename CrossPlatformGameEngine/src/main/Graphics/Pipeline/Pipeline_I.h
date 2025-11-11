#pragma once

#include "../HandleManager/Handle.h"

#include <vulkan/vulkan.h>

namespace ascen
{

	class Pipeline_I : public Handle<VkPipeline>
	{
	public:

		VkPipelineLayout getLayout() const
		{
			return mLayout;
		}

	protected:
		VkPipelineLayout mLayout{};
	};

}