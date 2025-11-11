#pragma once

#include "../Pipeline/GraphicsPipeline/GraphicsPipeline_I.h"
#include "../Pipeline/ComputePipeline/ComputePipeline_I.h"

#include <variant>
#include <vector>

namespace ascen
{

	enum class ResourceType
	{
		NONE   = 0,
		BUFFER = 1,
		IMAGE  = 2,
	};

	enum class Access
	{
		NONE  = 0,
		READ  = 1,
		WRITE = 2,
	};

	enum class Stage
	{
		NONE    = 0,
		VERTEX  = 1,
		PIXEL   = 2,
		COMPUTE = 4,
	};

	struct RenderGraphNode
	{
		ResourceType mResourceType = ResourceType::NONE;
		Access mAccess = Access::NONE;
		Stage mStage= Stage::NONE;
		Pipeline_I* mPipeline = nullptr;
		std::variant<VkBuffer, VkImage> mResource;
	};

	class RenderGraph
	{
	public:
		RenderGraph();

		void compile();

	private:
		std::vector<RenderGraphNode> mNodes;
	};

}
