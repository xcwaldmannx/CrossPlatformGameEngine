#include "Renderer.h"

#include "../../WindowManager/WindowManager.h"
#include "../../EcsSystem/EcsSystem.h"
#include "VulkanContext.h"
#include "RenderContext.h"
#include "../CommandPool/CommandPool.h"
#include "../Swapchain/Swapchain.h"
#include "../Resource/Buffer/Buffer.h"
#include "../Resource/Buffer/Indirect/IndirectBuffer.h"
#include "../Registry/Resource/ResourceRegistry.h"
#include "../Registry/Descriptor/DescriptorRegistry.h"
#include "../Registry/FramePass/FramePassRegistry.h"

using namespace ascen;

Renderer::Renderer(
	WindowManager& windowManager,
	EcsSystem& ecsSystem,
	VulkanContext& vulkanContext,
	RenderContext& renderContext,
	ResourceRegistry& resourceRegistry,
	DescriptorRegistry& descriptorRegistry,
	FramePassRegistry& framePassRegistry) :
	mWindowManager(windowManager),
	mEcsSystem(ecsSystem),
	mPhysicalDevice(vulkanContext.getPhysicalDevice()),
	mGraphicsQueue(vulkanContext.getGraphicsQueue()),
	mPresentQueue(vulkanContext.getPresentQueue()),
	mDevice(vulkanContext.getDevice()),
	mRenderContext(renderContext),
	mResourceRegistry(resourceRegistry),
	mDescriptorRegistry(descriptorRegistry),
	mFrameGraph(framePassRegistry)
{
	mResourceRegistry.registerBuffer({ "instance", ascen::BufferType::STORAGE, 1'000'000, sizeof(GPUInstance)});

	mDescriptorRegistry.registerDescriptor(
		{ "instance", "engine", 0x00, VK_WHOLE_SIZE, ascen::DescriptorType::SSBO, ascen::DescriptorStage::VERTEX});

	mIndirectBuffer = vulkanContext.getBufferFactory().createIndirect(renderContext.getCommandPool(), 1'000'000);

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
		mIndirectBuffer->update(
			mPhysicalDevice, mDevice, mGraphicsQueue, mRenderContext.getCommandPool(),
			drawCommands.data(), drawCommands.size(), sizeof(IndirectBuffer::DrawCommand));
	}
}

void Renderer::drawFrame()
{
	updateRenderSystem();

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
		mRenderContext.resize();
		return;
	}
	else if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swapchain image!");
	}

	vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

	mFrameGraph.compile();

	mRenderContext.getCommandPool()->record(
		mPhysicalDevice,
		mCurrentFrame,
		mCurrentImage,
		mFrameGraph,
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
		mRenderContext.resize();
		return;
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
	mIndirectBuffer->destroy(mDevice);
	destroySyncObjects();
}

uint32_t Renderer::getCurrentFrame() const
{
	return mCurrentFrame;
}
