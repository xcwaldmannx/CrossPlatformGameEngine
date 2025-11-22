#include "Renderer.h"

using namespace ascen;

Renderer::Renderer(
	WindowManager& windowManager,
	EcsSystem& ecsSystem,
	VulkanContext& vulkanContext,
	RenderContext& renderContext,
	ResourceRegistry& resourceRegistry,
	DescriptorRegistry& descriptorRegistry) :
	mWindowManager(windowManager),
	mEcsSystem(ecsSystem),
	mPhysicalDevice(vulkanContext.getPhysicalDevice()),
	mGraphicsQueue(vulkanContext.getGraphicsQueue()),
	mPresentQueue(vulkanContext.getPresentQueue()),
	mDevice(vulkanContext.getDevice()),
	mRenderContext(renderContext),
	mResourceRegistry(resourceRegistry),
	mDescriptorRegistry(descriptorRegistry)
{
	mResourceRegistry.registerBuffer({ "instance", ascen::BufferType::STORAGE, 1'000'000, sizeof(GPUInstance)});

	mDescriptorRegistry.registerDescriptor(
		{ "engine", "instance", 0x00, sizeof(GPUInstance), ascen::DescriptorType::SSBO, ascen::DescriptorStage::VERTEX});

	createSyncObjects();
}

void Renderer::updateRenderSystem()
{
	mEcsSystem.updateSystem<RenderSystem>(0);
	auto renderSystem = mEcsSystem.getSystem<RenderSystem>();
	const auto& instances = renderSystem->getInstances();
	const auto& drawCommands = renderSystem->getDrawCommands();

	mDrawCommandCount = drawCommands.size();

	if (!instances.empty())
	{
		mResourceRegistry.updateBuffer(
			"instance", instances.data(), instances.size(), sizeof(GPUInstance));
	}

	if (!drawCommands.empty())
	{
		mResourceRegistry.updateBuffer(
			"drawCommand", drawCommands.data(), drawCommands.size(), sizeof(IndirectBuffer::DrawCommand));
	}
}

void Renderer::drawFrame()
{
	bool isResized = false;
	vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

	VkResult nextImageResult = vkAcquireNextImageKHR(
		mDevice,
		mRenderContext.getSwapchain()->handle(),
		UINT64_MAX,
		mImageAvailableSemaphores[mCurrentFrame],
		VK_NULL_HANDLE,
		&mCurrentImage);

	if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		isResized = true;
		return;
	}
	else if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swapchain image!");
	}

	vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

	mRenderGraph.compile();

	mRenderContext.getCommandPool()->record(
		mPhysicalDevice,
		mCurrentFrame,
		mCurrentImage,
		mRenderGraph,
		mIndirectBuffer->handle(),
		mRenderContext.getRenderPass(),
		mRenderContext.getSwapchain(),
		mDrawCommandCount);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = mRenderContext.getCommandPool()->getBufferIndex(mCurrentFrame);

	VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapchains[] = { mRenderContext.getSwapchain()->handle() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &mCurrentImage;
	presentInfo.pResults = nullptr;

	VkResult queuePresentResult = vkQueuePresentKHR(mPresentQueue, &presentInfo);

	if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR)
	{
		isResized = true;
	}
	else if (queuePresentResult != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::updateModels(std::unordered_map<uint32_t, ModelData>& modelData)
{
	auto renderSystem = mEcsSystem.getSystem<RenderSystem>();
	renderSystem->updateModels(modelData);
}

void Renderer::createSyncObjects()
{
	mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS)
		{

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void Renderer::destroySyncObjects()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
	}
}

void Renderer::cleanup()
{
	destroySyncObjects();
}

void Renderer::createRenderPass(const std::string& name, const Pass& pass)
{
	RenderGraph::Pass renderGraphPass{};

	for (const auto& buffer : pass.mVertexBuffers)
	{
		if (mBuffers.find(buffer) != mBuffers.end())
		{
			renderGraphPass.mVertexBuffers.push_back(mBuffers[buffer]->handle());
		}
	}

	if (mBuffers.find(pass.mIndexBuffer) != mBuffers.end())
	{
		renderGraphPass.mIndexBuffer = mBuffers[pass.mIndexBuffer]->handle();
	}

	renderGraphPass.mDescriptorSets.push_back(mDescriptorSets["engine"]->handle());

	for (const auto& descriptorSet : pass.mDescriptorSets)
	{
		if (mDescriptorSets.find(descriptorSet) != mDescriptorSets.end())
		{
		 renderGraphPass.mDescriptorSets.push_back(mDescriptorSets[descriptorSet]->handle());
		}
	}

	if (!pass.mGraphicsPipeline.empty() &&
		mGraphicsPipelines.find(pass.mGraphicsPipeline) != mGraphicsPipelines.end())
	{
		renderGraphPass.mPipeline = mGraphicsPipelines[pass.mGraphicsPipeline]->handle();
		renderGraphPass.mPipelineLayout = mGraphicsPipelines[pass.mGraphicsPipeline]->getLayout();
	}
	else if (!pass.mComputePipeline.empty() &&
		mComputePipelines.find(pass.mComputePipeline) != mComputePipelines.end())
	{
		renderGraphPass.mPipeline = mComputePipelines[pass.mComputePipeline]->handle();
		renderGraphPass.mPipelineLayout = mComputePipelines[pass.mComputePipeline]->getLayout();
	}

	mRenderGraph.addPass(renderGraphPass);
}
