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
		VkPipelineLayout mLayout{};

		std::unordered_map<uint32_t, pipeline::PushConstant> mPushConstants;
	};

}