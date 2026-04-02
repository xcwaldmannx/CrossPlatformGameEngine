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
#include "../Resource/Buffer/Indirect/IndirectBuffer.h"
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
	/*
	const ascen::VertexBinding bindingVec3{ 0, sizeof(float) * 8, VK_VERTEX_INPUT_RATE_VERTEX };
	const std::vector<ascen::VertexAttribute> attributesVec3 =
	{
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
		{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3 },
		{ 2, 0, VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 6 }
	};

	mVertexRegistry.registerVertex({ "ENGINE_VERTEX_VEC3", bindingVec3, attributesVec3 });

	const ascen::VertexBinding bindingVec3Color{ 0, sizeof(float) * 6, VK_VERTEX_INPUT_RATE_VERTEX };
	const std::vector<ascen::VertexAttribute> attributesVec3Color =
	{
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
		{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3 },
	};

	mVertexRegistry.registerVertex({ "ENGINE_VERTEX_VEC3_COLOR", bindingVec3Color, attributesVec3Color });

	mResourceRegistry.registerBuffer({ "ENGINE_BUFFER_VERTEX_BBOX", ascen::BufferType::VERTEX, 1'000'000, sizeof(float) * 8 });

	mResourceRegistry.registerBuffer({ "ENGINE_BUFFER_VERTEX", ascen::BufferType::VERTEX, 1'000'000, sizeof(float) * 8 });
	mResourceRegistry.registerBuffer({ "ENGINE_BUFFER_INDEX", ascen::BufferType::INDEX, 1'000'000, sizeof(uint32_t) });

	mResourceRegistry.registerBuffer({ "ENGINE_BUFFER_CAMERA", BufferType::UNIFORM, 2, sizeof(glm::mat4) * 2 });
	mResourceRegistry.registerBuffer({ "ENGINE_BUFFER_ENTITY", BufferType::STORAGE, 1'000'000, sizeof(GPUEntity) });
	mResourceRegistry.registerBuffer({ "ENGINE_BUFFER_MESH", BufferType::STORAGE, 1'000'000, sizeof(GPUMesh) });
	mResourceRegistry.registerBuffer({ "ENGINE_BUFFER_TRANSFORM", BufferType::STORAGE, 1'000'000, sizeof(float) });
	mResourceRegistry.registerBuffer({ "ENGINE_BUFFER_INSTANCE", BufferType::STORAGE, 1'000'000, sizeof(GPUInstance) });
	mResourceRegistry.registerBuffer({ "ENGINE_BUFFER_DRAW", BufferType::INDIRECT, 1'000'000, sizeof(IndirectBuffer::IndexedIndirectCommand)});
	mResourceRegistry.registerBuffer({ "ENGINE_BUFFER_DRAW_BBOX", BufferType::INDIRECT, 1'000'000, sizeof(IndirectBuffer::IndexedIndirectCommand)});

	mResourceRegistry.registerSampler({ "ENGINE_SAMPLER" });
	mResourceRegistry.registerTexture({ "ENGINE_TEXTURE_IMAGE", TextureType::IMAGE, 1024, 1024, 16 });

	// ENGINE GRAPHICS

	// TRIANGLES

	mDescriptorRegistry.registerDescriptor(
		{ "ENGINE_BUFFER_CAMERA", "ENGINE_DESC_GRAPHICS", 0x00, sizeof(glm::mat4) * 2,
		DescriptorType::UBO_DYNAMIC, DescriptorStage::VERTEX });

	mDescriptorRegistry.registerDescriptor(
		{ "ENGINE_BUFFER_INSTANCE", "ENGINE_DESC_GRAPHICS", 0x01, VK_WHOLE_SIZE,
		DescriptorType::SSBO, DescriptorStage::VERTEX });

	mDescriptorRegistry.registerDescriptor(
		{ "ENGINE_SAMPLER", "ENGINE_DESC_GRAPHICS", 0x02, 0,
		DescriptorType::SAMPLER, DescriptorStage::PIXEL });

	mDescriptorRegistry.registerDescriptor(
		{ "ENGINE_TEXTURE_IMAGE", "ENGINE_DESC_GRAPHICS", 0x03, 0,
		DescriptorType::IMAGE, DescriptorStage::PIXEL });

	GraphicsPipelineEntry graphicsPipelineEntryTriangles;
	graphicsPipelineEntryTriangles.mName = "ENGINE_PIPELINE_GRAPHICS_TRIANGLES";
	graphicsPipelineEntryTriangles.mVertexShader = "src/shaders/GPUDrivenVS.spv";
	graphicsPipelineEntryTriangles.mPixelShader = "src/shaders/GPUDrivenPS.spv";
	graphicsPipelineEntryTriangles.mVertex = "ENGINE_VERTEX_VEC3";
	graphicsPipelineEntryTriangles.mDescriptorSetLayouts = { "ENGINE_DESC_GRAPHICS" };
	graphicsPipelineEntryTriangles.mParams.mTopologyMode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	graphicsPipelineEntryTriangles.mParams.mPolygonMode = VK_POLYGON_MODE_FILL;
	graphicsPipelineEntryTriangles.mParams.mCullMode = VK_CULL_MODE_BACK_BIT;

	mPipelineRegistry.registerGraphicsPipeline(graphicsPipelineEntryTriangles);

	// LINES

	mDescriptorRegistry.registerDescriptor(
		{ "ENGINE_BUFFER_CAMERA", "ENGINE_DESC_GRAPHICS_LINES", 0x00, sizeof(glm::mat4) * 2,
		DescriptorType::UBO_DYNAMIC, DescriptorStage::VERTEX });

	mDescriptorRegistry.registerDescriptor(
		{ "ENGINE_BUFFER_INSTANCE", "ENGINE_DESC_GRAPHICS_LINES", 0x01, VK_WHOLE_SIZE,
		DescriptorType::SSBO, DescriptorStage::VERTEX });

	GraphicsPipelineEntry graphicsPipelineEntryLines;
	graphicsPipelineEntryLines.mName = "ENGINE_PIPELINE_GRAPHICS_LINES";
	graphicsPipelineEntryLines.mVertexShader = "src/shaders/BoundingBoxVS.spv";
	graphicsPipelineEntryLines.mPixelShader = "src/shaders/BoundingBoxPS.spv";
	graphicsPipelineEntryLines.mVertex = "ENGINE_VERTEX_VEC3_COLOR";
	graphicsPipelineEntryLines.mDescriptorSetLayouts = { "ENGINE_DESC_GRAPHICS_LINES" };
	graphicsPipelineEntryLines.mParams.mTopologyMode = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	graphicsPipelineEntryLines.mParams.mPolygonMode = VK_POLYGON_MODE_FILL;
	graphicsPipelineEntryLines.mParams.mCullMode = VK_CULL_MODE_NONE;

	mPipelineRegistry.registerGraphicsPipeline(graphicsPipelineEntryLines);

	// ENGINE COMPUTE

	mDescriptorRegistry.registerDescriptor(
		{ "ENGINE_BUFFER_CAMERA", "ENGINE_DESC_COMPUTE", 0x00, sizeof(glm::mat4) * 2,
		DescriptorType::UBO_DYNAMIC, DescriptorStage::COMPUTE });

	mDescriptorRegistry.registerDescriptor(
		{ "ENGINE_BUFFER_ENTITY", "ENGINE_DESC_COMPUTE", 0x01, VK_WHOLE_SIZE,
		DescriptorType::SSBO, DescriptorStage::COMPUTE });

	mDescriptorRegistry.registerDescriptor(
		{ "ENGINE_BUFFER_MESH", "ENGINE_DESC_COMPUTE", 0x02, VK_WHOLE_SIZE,
		DescriptorType::SSBO, DescriptorStage::COMPUTE });

	mDescriptorRegistry.registerDescriptor(
		{ "ENGINE_BUFFER_TRANSFORM", "ENGINE_DESC_COMPUTE", 0x03, VK_WHOLE_SIZE,
		DescriptorType::SSBO, DescriptorStage::COMPUTE });

	mDescriptorRegistry.registerDescriptor(
		{ "ENGINE_BUFFER_INSTANCE", "ENGINE_DESC_COMPUTE", 0x04, VK_WHOLE_SIZE,
		DescriptorType::SSBO, DescriptorStage::COMPUTE });

	mPipelineRegistry.registerComputePipeline(
		{ "ENGINE_PIPELINE_COMPUTE", "src/shaders/GPUDrivenCS.spv", { "ENGINE_DESC_COMPUTE" } });


	// All FramePasses initialized here in order

	mFramePassRegistry.registerCompute(
	{ "ENGINE_FRAMEPASS_COMPUTE", { "ENGINE_DESC_COMPUTE" }, "ENGINE_PIPELINE_COMPUTE",
	{
		{ "ENGINE_BUFFER_CAMERA",    ResourceUsage::BUFFER_UNIFORM, ResourceAccess::READ,  ResourceStage::COMPUTE },
		{ "ENGINE_BUFFER_ENTITY",    ResourceUsage::BUFFER_STORAGE, ResourceAccess::READ,  ResourceStage::COMPUTE },
		{ "ENGINE_BUFFER_MESH",      ResourceUsage::BUFFER_STORAGE, ResourceAccess::READ,  ResourceStage::COMPUTE },
		{ "ENGINE_BUFFER_TRANSFORM", ResourceUsage::BUFFER_STORAGE, ResourceAccess::READ,  ResourceStage::COMPUTE },
		{"ENGINE_BUFFER_INSTANCE",   ResourceUsage::BUFFER_STORAGE, ResourceAccess::WRITE, ResourceStage::COMPUTE }
	},
	{ (100'000 + 63) / 64, 1, 1 }});

	mFramePassRegistry.registerGraphics(
		{ "ENGINE_FRAMEPASS_GRAPHICS_TRIANGLES", { "ENGINE_DESC_GRAPHICS" }, "ENGINE_PIPELINE_GRAPHICS_TRIANGLES",
		{
			{ "ENGINE_BUFFER_CAMERA",   ResourceUsage::BUFFER_UNIFORM, ResourceAccess::READ, ResourceStage::VERTEX   },
			{ "ENGINE_BUFFER_INSTANCE", ResourceUsage::BUFFER_STORAGE, ResourceAccess::READ, ResourceStage::VERTEX   },
			{ "ENGINE_TEXTURE_IMAGE",   ResourceUsage::IMAGE_SAMPLED,  ResourceAccess::READ, ResourceStage::FRAGMENT },
			{ "ENGINE_BUFFER_VERTEX",   ResourceUsage::BUFFER_VERTEX,  ResourceAccess::READ, ResourceStage::VERTEX   },
			{ "ENGINE_BUFFER_INDEX",    ResourceUsage::BUFFER_INDEX,   ResourceAccess::READ, ResourceStage::VERTEX   }
		},
		GraphicsMode::MESH });

	mFramePassRegistry.registerGraphics(
	{ "ENGINE_FRAMEPASS_GRAPHICS_LINES", { "ENGINE_DESC_GRAPHICS_LINES" }, "ENGINE_PIPELINE_GRAPHICS_LINES",
	{
		{ "ENGINE_BUFFER_CAMERA",      ResourceUsage::BUFFER_UNIFORM, ResourceAccess::READ, ResourceStage::VERTEX },
		{ "ENGINE_BUFFER_INSTANCE",    ResourceUsage::BUFFER_STORAGE, ResourceAccess::READ, ResourceStage::VERTEX },
		{ "ENGINE_BUFFER_VERTEX_BBOX", ResourceUsage::BUFFER_VERTEX,  ResourceAccess::READ, ResourceStage::VERTEX },
	},
	GraphicsMode::LINES });
	*/


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
				// const auto& drawBuffer = ResourceRegistryBackend::getBuffer(mResourceRegistry, "ENGINE_BUFFER_DRAW_BBOX");

				// mLineCommandRecorder.record(commandBuffer, pass, mFrameIndex, drawBuffer->handle(), mDrawCommandCount);
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
