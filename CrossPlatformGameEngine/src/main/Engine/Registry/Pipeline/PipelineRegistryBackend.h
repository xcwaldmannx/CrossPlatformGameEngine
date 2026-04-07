#pragma once

#include "PipelineRegistry.h"

namespace ascen
{

	class PipelineRegistryBackend
	{
	public:
		static const GraphicsPipelinePtr& getGraphicsPipeline(
			const PipelineRegistry& registry, const std::string& name);

		static const ComputePipelinePtr& getComputePipeline(
			const PipelineRegistry& registry, const std::string& name);
	};

}
