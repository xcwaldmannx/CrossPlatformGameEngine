#include "Renderer.h"

#include "../../WindowManager/WindowManager.h"
#include "../../EcsSystem/EcsSystem.h"
#include "VulkanContext.h"
#include "RenderContext.h"
#include "../CommandPool/CommandPool.h"
#include "../CommandRecorder/LineCommandRecorder/LineCommandRecorder.h"
#include "../CommandRecorder/MeshCommandRecorder/MeshCommandRecorder.h"
#include "../Swapchain/Swapchain.h"
#include "../FrameGraph/FramePass/GpuFramePass/GraphicsGpuFramePass/GraphicsGpuFramePass.h"
#include "../FrameGraph/FramePass/GpuFramePass/ComputeGpuFramePass/ComputeGpuFramePass.h"
#include "../Resource/Barrier/Barrier.h"
#include "../Resource/Buffer/Buffer.h"
#include "../Registry/Vertex/VertexRegistry.h"
#include "../Registry/Resource/ResourceRegistry.h"
#include "../Registry/Resource/ResourceRegistryBackend.h"
#include "../Registry/Descriptor/DescriptorRegistry.h"
#include "../Registry/Pipeline/PipelineRegistry.h"
#include "../Registry/FramePass/FramePassRegistry.h"

#include <filesystem>

#include <glm/glm.hpp>

using namespace ascen;

Renderer::Renderer(
	WindowManager& windowManager,
	EcsSystem& ecsSystem,
	VulkanContext& vulkanContext,
	RenderContext& renderContext,
	VertexRegistry& vertexRegistry,
	ResourceRegistry& resourceRegistry,
	DescriptorRegistry& descriptorRegistry,
	PipelineRegistry& pipelineRegistry,
	FramePassRegistry& framePassRegistry) :
	mWindowManager(windowManager),
	mEcsSystem(ecsSystem),
	mPhysicalDevice(vulkanContext.getPhysicalDevice()),
	mGraphicsQueue(vulkanContext.getGraphicsQueue()),
	mPresentQueue(vulkanContext.getPresentQueue()),
	mDevice(vulkanContext.getDevice()),
	mRenderContext(renderContext),
	mVertexRegistry(vertexRegistry),
	mResourceRegistry(resourceRegistry),
	mDescriptorRegistry(descriptorRegistry),
	mPipelineRegistry(pipelineRegistry),
	mFramePassRegistry(framePassRegistry),
	mFrameGraph(mFramePassRegistry, mResourceRegistry),
	mLineCommandRecorder(mPipelineRegistry, mDescriptorRegistry, mResourceRegistry),
	mMeshCommandRecorder(mPipelineRegistry, mDescriptorRegistry, mResourceRegistry),
	mComputeCommandRecorder(mPipelineRegistry, mDescriptorRegistry, mResourceRegistry)
{
	createSyncObjects();
}

void Renderer::drawFrame()
{
	if (mWindowManager.getWidth() == 0 ||
		mWindowManager.getHeight() == 0)
	{
		return;
	}

	const auto& commandPool = mRenderContext.getCommandPool();
	const auto& renderPass = mRenderContext.getRenderPass();
	const auto& swapchain = mRenderContext.getSwapchain();

	vkWaitForFences(mDevice, 1, &mInFlightFences[mFrameIndex], VK_TRUE, UINT64_MAX);

	const VkResult nextImageResult = vkAcquireNextImageKHR(
		mDevice,
		swapchain->handle(),
		UINT64_MAX,
		mImageAvailableSemaphores[mFrameIndex],
		VK_NULL_HANDLE,
		&mImageIndex);

	if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		mRenderContext.resize();
		return;
	}

	if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swapchain image!");
	}

	vkResetFences(mDevice, 1, &mInFlightFences[mFrameIndex]);

	mFrameGraph.compile();
	const auto& executions = mFrameGraph.getExecutions();

	const auto commandBuffer = commandPool->beginCommand(mFrameIndex);

	// upload all push constants
	mPipelineRegistry.uploadAllPushConstants(commandBuffer);

	bool isRenderPassActive = false;

	for (const auto& exec : executions)
	{
		switch (exec->mType)
		{
		case FramePassType::GRAPHICS:
		{
			if (!isRenderPassActive)
			{
				commandPool->beginRenderPass(commandBuffer, mImageIndex, renderPass, swapchain);
				isRenderPassActive = true;
			}

			const auto& pass = reinterpret_cast<const GraphicsGpuFramePass*>(exec.get());

			if (pass->mMode == GraphicsMode::MESH)
			{
				mMeshCommandRecorder.record(commandBuffer, pass, mFrameIndex);
			}
			else if (pass->mMode == GraphicsMode::LINES)
			{
				mLineCommandRecorder.record(commandBuffer, pass, mFrameIndex);
			}
			break;
		}
		case FramePassType::COMPUTE:
		{
			if (isRenderPassActive)
			{
				commandPool->endRenderPass(commandBuffer);
				isRenderPassActive = false;
			}

			const auto& pass = reinterpret_cast<const ComputeGpuFramePass*>(exec.get());

			mComputeCommandRecorder.record(commandBuffer, pass, mFrameIndex);
			break;
		}
		case FramePassType::SYNC:
		{
			const auto& pass = reinterpret_cast<const SyncFramePass*>(exec.get());
			Barrier::buffer(
				commandBuffer,
				pass->mBuffer,
				pass->mSrcAccess,
				pass->mSrcStage,
				pass->mDstAccess,
				pass->mDstStage);
		}
			break;
		case FramePassType::NONE:
		{

		}
		default:
		{
			return;
		}
		}
	}

	if (isRenderPassActive)
	{
		commandPool->endRenderPass(commandBuffer);
		isRenderPassActive = false;
	}

	commandPool->endCommand(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mFrameIndex] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT }; // previously was VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandPool->getBufferIndex(mFrameIndex);

	VkSemaphore signalSemaphores[] = { mRenderFinishedForImageSemaphores[mImageIndex] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mFrameIndex]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapchains[] = { swapchain->handle() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &mImageIndex;
	presentInfo.pResults = nullptr;

	VkResult queuePresentResult = vkQueuePresentKHR(mPresentQueue, &presentInfo);

	if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
		queuePresentResult == VK_SUBOPTIMAL_KHR ||
		mWindowManager.isResized())
	{
		mRenderContext.resize();
		return;
	}
	else if (queuePresentResult != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	mFrameIndex = (mFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::createSyncObjects()
{
	mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	// mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
			//vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS)
		{

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}

	size_t swapchainImageCount = mRenderContext.getSwapchain()->getImageCount();
	mRenderFinishedForImageSemaphores.resize(swapchainImageCount);

	for (size_t i = 0; i < swapchainImageCount; i++)
	{
		if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedForImageSemaphores[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void Renderer::destroySyncObjects()
{
	for (size_t i = 0; i < mRenderFinishedForImageSemaphores.size(); i++)
	{
		vkDestroySemaphore(mDevice, mRenderFinishedForImageSemaphores[i], nullptr);
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		//vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
	}
}

void Renderer::cleanup()
{
	destroySyncObjects();
}

uint32_t Renderer::getFrameIndex() const
{
	return mFrameIndex;
}
