#include "FrameGraph.h"

#include "../Registry/FramePass/FramePassRegistryBackend.h"

using namespace ascen;

FrameGraph::FrameGraph(const FramePassRegistry& framePassRegistry) :
	mFramePassRegistry(framePassRegistry) {}

void FrameGraph::compile()
{
	mExecutions.clear();
}

const std::vector<FramePass>& FrameGraph::getExecutions() const
{
	return mExecutions;
}
