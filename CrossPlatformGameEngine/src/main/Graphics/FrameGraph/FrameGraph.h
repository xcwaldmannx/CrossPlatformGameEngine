#pragma once

#include "FramePass.h"

#include "../Registry/FramePass/FramePassRegistry.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class FrameGraph
	{
	public:
		FrameGraph(const FramePassRegistry& framePassRegistry);

		void compile();

		const std::vector<FramePass>& getExecutions() const;

	private:
		const FramePassRegistry& mFramePassRegistry;

		std::vector<FramePass> mExecutions;
	};

}
