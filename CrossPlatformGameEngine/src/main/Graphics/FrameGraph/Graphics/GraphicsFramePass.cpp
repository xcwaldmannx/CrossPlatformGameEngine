#include "GraphicsFramePass.h"

using namespace ascen;

GraphicsFramePass::GraphicsFramePass(
	const FramePassType type,
	const GraphicsFramePassMode mode,
	const std::vector<VkDescriptorSet> descriptorSets,
	const VkPipeline pipeline,
	const VkPipelineLayout pipelineLayout,
	const std::vector<VkBuffer> readBuffers,
	const std::vector<VkBuffer> writeBuffers,
	const std::vector<VkImageView> readTextures,
	const std::vector<VkImageView> writeTextures,
	const std::vector<VkBuffer> vertexBuffers,
	const VkBuffer indexBuffer) :
	FramePass(type, descriptorSets, pipeline, pipelineLayout,
		readBuffers, writeBuffers, readTextures, writeTextures),
	mVertexBuffers(vertexBuffers),
	mIndexBuffer(indexBuffer),
	mMode(mode) {}