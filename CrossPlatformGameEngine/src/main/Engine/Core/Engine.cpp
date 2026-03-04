#include "Engine.h"

#include "../../WindowManager/WindowManager.h"

#include "../Swapchain/Swapchain.h"

using namespace ascen;

Engine::Engine(WindowManager& windowManager) :
	mWindowManager(windowManager),
	mVulkanContext(windowManager),
	mRenderContext(windowManager, mVulkanContext),
	mResourceRegistry(mVulkanContext, mRenderContext),
	mDescriptorRegistry(mVulkanContext, mResourceRegistry),
	mPipelineRegistry(mVulkanContext, mRenderContext, mVertexRegistry, mDescriptorRegistry),
	mFramePassRegistry(mResourceRegistry, mDescriptorRegistry, mPipelineRegistry),
	mRenderer(windowManager, mEcs, mVulkanContext, mRenderContext,
		mVertexRegistry, mResourceRegistry, mDescriptorRegistry, mPipelineRegistry, mFramePassRegistry)
{
	// initialize ECS
	mEcs.registerComponent<TransformComponent>();
	mEcs.registerComponent<ModelComponent>();

	std::unordered_map<uint32_t, ModelData> modelData;

	auto readSig = mEcs.getSignature<TransformComponent, ModelComponent>();
	auto writeSig = mEcs.getSignature<ModelComponent>();
	mEcs.registerSystem<RenderSystem>(readSig, writeSig, &modelData);
}

VertexRegistry& Engine::vertex()
{
	return mVertexRegistry;
}

ResourceRegistry& Engine::resource()
{
	return mResourceRegistry;
}

DescriptorRegistry& Engine::descriptor()
{
	return mDescriptorRegistry;
}

PipelineRegistry& Engine::pipeline()
{
	return mPipelineRegistry;
}

FramePassRegistry& Engine::frame()
{
	return mFramePassRegistry;
}

EcsSystem& Engine::ecs()
{
	return mEcs;
}

void Engine::reload()
{
	mVertexRegistry.reconstruct();
	mResourceRegistry.reconstruct();
	mDescriptorRegistry.reconstruct();
	mPipelineRegistry.reconstruct();
	mFramePassRegistry.reconstruct();
}

void Engine::updateModelData(std::unordered_map<uint32_t, ModelData>& modelData)
{
	mRenderer.updateModels(modelData);
}

void Engine::drawFrame()
{
	mRenderer.drawFrame();
}

void Engine::cleanup()
{
	mVulkanContext.waitIdle();

	mRenderer.cleanup();
	mFramePassRegistry.cleanup();
	mPipelineRegistry.cleanup();
	mDescriptorRegistry.cleanup();
	mResourceRegistry.cleanup();
	mVertexRegistry.cleanup();
	mRenderContext.cleanup();
	mVulkanContext.cleanup();
}

uint32_t Engine::getScreenWidth() const
{
	return mRenderContext.getSwapchain()->getExtent().width;
}

uint32_t Engine::getScreenHeight() const
{
	return mRenderContext.getSwapchain()->getExtent().height;
}

uint32_t Engine::getFrameIndex() const
{
	return mRenderer.getFrameIndex();
}
