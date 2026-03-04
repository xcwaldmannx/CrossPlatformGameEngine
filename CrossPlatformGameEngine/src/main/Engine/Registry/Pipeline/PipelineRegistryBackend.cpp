#include "PipelineRegistryBackend.h"

#include <stdexcept>

using namespace ascen;

const GraphicsPipelinePtr& PipelineRegistryBackend::getGraphicsPipeline(
	const PipelineRegistry& registry, const std::string& name)
{
	if (registry.graphicsPipelineExists(name))
	{
		return registry.mGraphicsPipelines.at(name);
	}
	else
	{
		throw std::runtime_error("Graphics pipeline does not exist!");
	}
}

const ComputePipelinePtr& PipelineRegistryBackend::getComputePipeline(
	const PipelineRegistry& registry, const std::string& name)
{
	if (registry.computePipelineExists(name))
	{
		return registry.mComputePipelines.at(name);
	}
	else
	{
		throw std::runtime_error("Compute pipeline does not exist!");
	}
}
