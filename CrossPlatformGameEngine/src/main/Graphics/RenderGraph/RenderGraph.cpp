#include "RenderGraph.h"

using namespace ascen;

RenderGraph::RenderGraph()
{

}

void RenderGraph::addPass(const RenderGraph::Pass& pass)
{
	mPasses.push_back(pass);
}

void RenderGraph::compile()
{
	mExecutions.clear();
	for (const auto& pass : mPasses)
	{
		mExecutions.push_back(pass);
	}
}

const std::vector<RenderGraph::Pass>& RenderGraph::getExecutions() const
{
	return mExecutions;
}
