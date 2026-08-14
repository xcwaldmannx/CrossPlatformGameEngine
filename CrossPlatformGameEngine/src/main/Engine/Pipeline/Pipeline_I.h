#pragma once

#include "../Handle/Handle.h"

#include <cstring>

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

		void destroy(VkDevice device) override = 0;

		template<typename T>
		void updatePushConstant(const uint32_t pushConstantId, const T& data)
		{
			auto& pushConstant = mPushConstants.at(pushConstantId);
			const size_t dataSize = sizeof(T);

			if (dataSize > pushConstant.mSize)
			{
				throw std::runtime_error("Push constant range too small for data type.");
			}

			if (!pushConstant.mData) {
				pushConstant.mData = std::make_shared<char[]>(pushConstant.mSize);
			}

			std::memcpy(pushConstant.mData.get(), &data, dataSize);
		}

	protected:
		VkPipelineLayout mLayout{};

		std::unordered_map<uint32_t, pipeline::PushConstant> mPushConstants;
	};

}