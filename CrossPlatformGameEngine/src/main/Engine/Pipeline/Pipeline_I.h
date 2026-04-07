#pragma once

#include "../Handle/Handle.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	struct PushConstantRange
	{
		std::string mName = "";
		uint32_t mOffset = 0;
		uint32_t mSize = 0;
	};

	struct PipelineParams
	{
		PushConstantRange mPushConstantRange{};
	};

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