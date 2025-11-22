#include "Engine.h"

using namespace ascen;

Engine::Engine(WindowManager& windowManager) :
	mWindowManager(windowManager),
	mVulkanContext(windowManager),
	mRenderContext(windowManager, mVulkanContext),
	mResourceRegistry(mVulkanContext, mRenderContext),
	mDescriptorRegistry(mVulkanContext, mResourceRegistry),
	mPipelineRegistry(mVulkanContext, mRenderContext, mVertexRegistry, mDescriptorRegistry),
	mFramePassRegistry(mResourceRegistry, mDescriptorRegistry, mPipelineRegistry),
	mRenderer(windowManager, mEcs, mVulkanContext, mRenderContext, mResourceRegistry, mDescriptorRegistry)
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

EcsSystem& Engine::ecs()
{
	return mEcs;
}

void Engine::cleanup()
{
	mVulkanContext.waitIdle();

	mRenderer.cleanup();
	mDescriptorRegistry.cleanup();
	mResourceRegistry.cleanup();
	mRenderContext.cleanup();
	mVulkanContext.cleanup();
}
