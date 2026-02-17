#include "FrameGraph.h"

#include "../Registry/FramePass/FramePassRegistryBackend.h"

#include <algorithm>

using namespace ascen;

FrameGraph::FrameGraph(const FramePassRegistry& framePassRegistry) :
	mFramePassRegistry(framePassRegistry) {}

void FrameGraph::compile()
{
	mExecutions.clear();

	const auto& framePasses = FramePassRegistryBackend::getFramePasses(mFramePassRegistry);

	for (auto& [name, framePass] : framePasses)
	{
		mExecutions.push_back(framePass);
	}

	std::reverse(mExecutions.begin(), mExecutions.end());
}

const std::vector<FramePassPtr>& FrameGraph::getExecutions() const
{
	return mExecutions;
}
