#include "FramePass.h"

using namespace ascen;

FramePass::FramePass(
	const FramePassType type,
	const std::vector<VkDescriptorSet> descriptorSets,
	const VkPipeline pipeline,
	const VkPipelineLayout pipelineLayout,
	const std::vector<VkBuffer> readBuffers,
	const std::vector<VkBuffer> writeBuffers,
	const std::vector<VkImageView> readTextures,
	const std::vector<VkImageView> writeTextures) :
	mType(type),
	mDescriptorSets(descriptorSets),
	mPipeline(pipeline),
	mPipelineLayout(pipelineLayout),
	mReadBuffers(readBuffers),
	mWriteBuffers(writeBuffers),
	mReadTextures(readTextures),
	mWriteTextures(writeTextures) {}