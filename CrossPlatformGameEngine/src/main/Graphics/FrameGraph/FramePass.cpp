#include "FramePass.h"

using namespace ascen;

FramePass::FramePass(
	const std::vector<VkBuffer> vertexBuffers,
	const VkBuffer indexBuffer,
	const std::vector<VkBuffer> readBuffers,
	const std::vector<VkBuffer> writeBuffers,
	const std::vector<VkImageView> readTextures,
	const std::vector<VkImageView> writeTextures,
	const std::vector<VkDescriptorSet> descriptorSets,
	const VkPipeline pipeline,
	const VkPipelineLayout pipelineLayout) :
	mVertexBuffers(vertexBuffers),
	mIndexBuffer(indexBuffer),
	mReadBuffers(readBuffers),
	mWriteBuffers(writeBuffers),
	mReadTextures(readTextures),
	mWriteTextures(writeTextures),
	mDescriptorSets(descriptorSets),
	mPipeline(pipeline),
	mPipelineLayout(pipelineLayout) {}