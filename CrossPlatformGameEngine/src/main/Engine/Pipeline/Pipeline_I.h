#pragma once

#include "../Handle/Handle.h"

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

		virtual void destroy(VkDevice device) override = 0;

	protected:
		VkPushConstantRange mPushConstantRange{};

		VkPipelineLayout mLayout{};
	};

}