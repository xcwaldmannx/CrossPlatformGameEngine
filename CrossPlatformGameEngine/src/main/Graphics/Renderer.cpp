#include "Renderer.h"

using namespace ascen;

Renderer::Renderer(WindowManager& windowManager, Engine& engine) :
	mWindowManager(windowManager),
	mEngine(engine)
{
	mCommandPool = mEngine.commandPool().create(mEngine.getGraphicsFamily());

	mSwapchain = mEngine.swapchain().create(
		mWindowManager.getWindow(), mEngine.getGraphicsFamily(), mEngine.getPresentFamily());

	mRenderPass = mEngine.renderPass().create(VK_FORMAT_R8G8B8A8_SRGB);
}

void Renderer::createResourceLayouts(std::vector<ResourceLayout>& layouts)
{
	std::unordered_map<VkDescriptorType, uint32_t> poolCounts{};
	std::vector<DescriptorSetLayout::Binding> bindings;
	std::vector<DescriptorSet::Write> writes;

	for (const auto& layout : layouts)
	{
		poolCounts[static_cast<VkDescriptorType>(layout.mType)]++;

		bindings.push_back(mEngine.descriptor().createBinding(
			layout.mSlot,
			static_cast<VkDescriptorType>(layout.mType),
			static_cast<uint32_t>(layout.mStage)));

		switch (layout.mType)
		{
		case ResourceType::IMAGE:
			{
				if (mTextures.find(layout.mName) == mTextures.end())
				{
					throw std::runtime_error("Image does not exist!");
				}

				const TexturePtr& texture = mTextures[layout.mName];
				writes.push_back(mEngine.descriptor().createImageWrite(
					static_cast<VkDescriptorType>(layout.mType),
					texture->handle(),
					VK_NULL_HANDLE,
					layout.mSlot));
				break;
			}
		case ResourceType::SAMPLER:
		case ResourceType::IMAGE_SAMPLER:
			break;
		default:
			{
				if (mBuffers.find(layout.mName) == mBuffers.end())
				{
					throw std::runtime_error("Buffer does not exist!");
				}

				const BufferPtr& buffer = mBuffers[layout.mName];
				writes.push_back(mEngine.descriptor().createBufferWrite(
					static_cast<VkDescriptorType>(layout.mType),
					buffer->handle(),
					0,
					static_cast<VkDeviceSize>(layout.mSize),
					layout.mSlot));
			}
		}
	}

	std::vector<DescriptorPool::Size> poolSizes;

	for (auto& [type, count] : poolCounts)
	{
		poolSizes.emplace_back(static_cast<VkDescriptorType>(type), count);
	}

	mDescriptorPool = mEngine.descriptor().createPool(poolSizes);
	mDescriptorSetLayout = mEngine.descriptor().createLayout(bindings);
	mDescriptorSet = mEngine.descriptor().createSet(mDescriptorPool, mDescriptorSetLayout, writes);
}

void Renderer::createComputePipeline(
	const std::string& name,
	const std::string& computeShaderFilepath)
{
	if (mComputePipelines.find(name) == mComputePipelines.end())
	{
		mComputePipelines[name] = mEngine.computePipeline().create(
			computeShaderFilepath, mDescriptorSetLayout);
	}
	else
	{
		throw std::runtime_error("A compute pipeline with that name already exists!");
	}
}

void Renderer::resize()
{
	mEngine.resize(mCommandPool, mRenderPass, mSwapchain, mDepthTexture);
}

/*
void Renderer::drawFrame()
{
	vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);
	
	uint32_t currentImage;
	VkResult nextImageResult = vkAcquireNextImageKHR(
		mDevice,
		mSwapchain->handle(),
		UINT64_MAX,
		mImageAvailableSemaphores[mCurrentFrame],
		VK_NULL_HANDLE,
		&currentImage);
	
	if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		mIsWindowResized = true;
		return;
	}
	else if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swapchain image!");
	}
	
	vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);
	
	record(currentImage);
	
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = mCommandPool->getBufferIndex(mCurrentFrame);
	
	VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	
	VkQueue graphicsQueue = ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value());
	
	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}
	
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	
	VkSwapchainKHR swapChains[] = { mSwapchain->handle() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &currentImage;
	presentInfo.pResults = nullptr;
	
	VkQueue presentQueue = ascen::QueueFamilies::getDeviceQueue(mDevice, mPresentFamily.value());
	VkResult queuePresentResult = vkQueuePresentKHR(presentQueue, &presentInfo);
	
	if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR)
	{
		mIsWindowResized = true;
	}
	else if (queuePresentResult != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}
	
	mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	
	mDrawCommands.clear();
}
*/

void Renderer::cleanup()
{
	for (const auto& [name, pipeline] : mGraphicsPipelines)
	{
		if (pipeline) mEngine.destroy<GraphicsPipeline_I>(*pipeline);
	}

	for (const auto& [name, pipeline] : mComputePipelines)
	{
		if (pipeline) mEngine.destroy<ComputePipeline_I>(*pipeline);
	}

	if (mRenderPass) mEngine.destroy<RenderPass>(*mRenderPass);
	if (mSwapchain) mEngine.destroy<Swapchain>(*mSwapchain);
	if (mDepthTexture) mEngine.destroy<Texture2>(*mDepthTexture);

	for (const auto& [name, buffer] : mBuffers)
	{
		if (buffer) mEngine.destroy<Buffer2>(*buffer);
	}

	for (const auto& [name, texture] : mTextures)
	{
		if (texture) mEngine.destroy<Texture2>(*texture);
	}

	if (mDescriptorPool) mEngine.destroy<DescriptorPool>(*mDescriptorPool);
	if (mDescriptorSetLayout) mEngine.destroy<DescriptorSetLayout>(*mDescriptorSetLayout);
	if (mCommandPool) mEngine.destroy<CommandPool>(*mCommandPool);
}
