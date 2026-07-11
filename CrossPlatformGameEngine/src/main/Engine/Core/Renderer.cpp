#include "Renderer.h"

#include "../../WindowManager/WindowManager.h"
#include "../../EcsSystem/EcsSystem.h"
#include "VulkanContext.h"
#include "RenderContext.h"
#include "../CommandPool/CommandPool.h"
#include "../CommandRecorder/LineCommandRecorder/LineCommandRecorder.h"
#include "../CommandRecorder/MeshCommandRecorder/MeshCommandRecorder.h"
#include "../Swapchain/Swapchain.h"
#include "../Resource/Barrier/Barrier.h"
#include "../Resource/Buffer/Buffer.h"

#include <filesystem>

#include <glm/glm.hpp>

using namespace ascen;

Renderer::Renderer(
	WindowManager& windowManager,
	EcsSystem& ecsSystem,
	VulkanContext& vulkanContext,
	RenderContext& renderContext,
	RegistryManager& registryManager) :
	mWindowManager(windowManager),
	mEcsSystem(ecsSystem),
	mPhysicalDevice(vulkanContext.getPhysicalDevice()),
	mGraphicsQueue(vulkanContext.getGraphicsQueue()),
	mPresentQueue(vulkanContext.getPresentQueue()),
	mDevice(vulkanContext.getDevice()),
	mRenderContext(renderContext),
	mRegistryManager(registryManager)
{
	createSyncObjects();
}

void Renderer::drawFrame()
{
	if (WindowManager::getWidth() == 0 ||
		WindowManager::getHeight() == 0)
	{
		return;
	}

	const auto& swapchain = mRenderContext.getSwapchain();

	acquireNextFrame(swapchain);

	auto framePasses = mFrameGraph.compile(mRegistryManager.getResourceType<FramePass>());

	const auto& commandPool = mRenderContext.getCommandPool();
	const auto commandBuffer = commandPool->beginCommand(mFrameIndex);

	uint64_t currentRenderPass = 0;
	bool isRenderPassActive = false;

	for (const auto& framePass : framePasses)
	{
		switch (framePass->mType)
		{
			case FRAMEPASS_TYPE_GRAPHICS:
			{
				if (!isRenderPassActive || currentRenderPass != framePass->mRenderPassId)
				{
					if (isRenderPassActive)
					{
						commandPool->endRenderPass(commandBuffer);
					}

					const auto& renderPassPtr = mRegistryManager.getResource<RenderPass>(framePass->mRenderPassId);

					VkFramebuffer frameBuffer = VK_NULL_HANDLE;

					if (framePass->mFrameBufferId == 0) // use swapchain framebuffer
					{
						swapchain->createFrameBuffers(mDevice, renderPassPtr->handle());
						frameBuffer = swapchain->getFrameBuffer(mImageIndex);
					}
					else // use specific framebuffer
					{
						const auto& frameBufferPtr = mRegistryManager.getResource<FrameBuffer>(framePass->mFrameBufferId);
						frameBuffer = frameBufferPtr->handle();
					}

					VkExtent2D extent;

					if (framePass->mExtent.width == 0 && framePass->mExtent.height == 0) // use swapchain extent
					{
						extent = swapchain->getExtent();
					}
					else // use specific extent
					{
						extent = framePass->mExtent;
					}


					// temp
					// The compute pass writes entity visibility and generated bounds. Make
					// those writes visible to the vertex shader and vertex/index fetches.
					VkMemoryBarrier computeToGraphics{};
					computeToGraphics.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
					computeToGraphics.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
					computeToGraphics.dstAccessMask =
						VK_ACCESS_SHADER_READ_BIT |
						VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
						VK_ACCESS_INDEX_READ_BIT;

					vkCmdPipelineBarrier(
						commandBuffer,
						VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
						0,
						1,
						&computeToGraphics,
						0,
						nullptr,
						0,
						nullptr);
					// temp


					commandPool->beginRenderPass(commandBuffer, renderPassPtr, frameBuffer, extent);
					isRenderPassActive = true;
					currentRenderPass = framePass->mRenderPassId;
				}

				const auto& pipeline = mRegistryManager.getResource<GraphicsPipeline>(framePass->mPipelineId);

				std::vector<VkDescriptorSet> descriptorSets;
				for (const auto& id : framePass->mDescriptorSetIds)
				{
					const auto& set = mRegistryManager.getResource<DescriptorSet>(id);
					descriptorSets.push_back(set->handle());
				}

				std::vector<VkBuffer> vertexBuffers;
				for (const auto& id : framePass->mVertexBufferIds)
				{
					const auto& buffer = mRegistryManager.getResource<Buffer>(id);
					vertexBuffers.push_back(buffer->handle());
				}

				const auto& indexBufferPtr = mRegistryManager.getResource<Buffer>(framePass->mIndexBufferId);
				const VkBuffer indexBuffer = indexBufferPtr->handle();

				const auto& indirectBufferPtr = mRegistryManager.getResource<Buffer>(framePass->mIndirectBufferId);
				const VkBuffer indirectBuffer = indirectBufferPtr->handle();

				if (framePass->mDrawMode == FRAMEPASS_DRAW_MODE_TRIANGLES)
				{
					mMeshCommandRecorder.record(commandBuffer, pipeline, descriptorSets, vertexBuffers, indexBuffer, indirectBuffer, 1'000'000);
				}
				else if (framePass->mDrawMode == FRAMEPASS_DRAW_MODE_LINES)
				{
					mLineCommandRecorder.record(commandBuffer, pipeline, descriptorSets, vertexBuffers, indexBuffer);
				}
				break;
			}
			case FRAMEPASS_TYPE_COMPUTE:
			{
				if (isRenderPassActive)
				{
					commandPool->endRenderPass(commandBuffer);
					isRenderPassActive = false;
				}

				const auto& pipeline = mRegistryManager.getResource<ComputePipeline>(framePass->mPipelineId);

				std::vector<VkDescriptorSet> descriptorSets;
				for (const auto& id : framePass->mDescriptorSetIds)
				{
					const auto& set = mRegistryManager.getResource<DescriptorSet>(id);
					descriptorSets.push_back(set->handle());
				}

				mComputeCommandRecorder.record(commandBuffer, pipeline, descriptorSets, framePass->mComputeGroups);
				break;
			}
			default:
				break;
		}
	}

	if (isRenderPassActive)
	{
		commandPool->endRenderPass(commandBuffer);
		isRenderPassActive = false;
	}

	commandPool->endCommand(commandBuffer);

	submitFrame(commandPool);
	presentFrame(swapchain);

	mFrameIndex = (mFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::acquireNextFrame(const SwapchainPtr& swapchain)
{
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
		throw std::runtime_error("Failed to acquire swapchain image!");
	}

	vkResetFences(mDevice, 1, &mInFlightFences[mFrameIndex]);
}

void Renderer::submitFrame(const CommandPoolPtr& commandPool) const
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	const VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mFrameIndex] };
	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT }; // previously was VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
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
}
void Renderer::presentFrame(const SwapchainPtr& swapchain) const
{
	const VkSemaphore signalSemaphores[] = { mRenderFinishedForImageSemaphores[mImageIndex] };

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
