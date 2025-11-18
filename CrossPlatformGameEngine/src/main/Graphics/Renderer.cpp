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

	mEngine.resize(mCommandPool, mRenderPass, mSwapchain, mDepthTexture);

	std::vector<GPUInstance> dummmyInstances(1000);
	createStorageBuffer<GPUInstance>("instances", dummmyInstances);
	addResourceLayout({ "engine", "instances", 0x02, VK_WHOLE_SIZE, ResourceType::SSBO, ResourceStage::VERTEX });

	std::vector<IndirectBuffer::DrawCommand> dummyDrawCommands(1000);
	mIndirectBuffer = mEngine.buffer().createIndirect(
		mEngine.getGraphicsFamily(), mCommandPool, dummyDrawCommands);
}

void Renderer::finalize()
{
	createDescriptorSetLayouts();
}

void Renderer::createComputePipeline(
	const std::string& name,
	const std::vector<std::string>& descriptors,
	const std::string& computeShaderFilepath)
{
	if (mComputePipelines.find(name) == mComputePipelines.end())
	{
		std::vector<DescriptorSetLayoutPtr> descriptorSetLayouts;
		descriptorSetLayouts.push_back(mDescriptorSetLayouts.at("engine"));

		for (const auto& descriptor : descriptors)
		{
			if (mDescriptorSetLayouts.find(descriptor) != mDescriptorSetLayouts.end())
			{
				descriptorSetLayouts.push_back(mDescriptorSetLayouts.at(descriptor));
			}
		}

		mComputePipelines[name] = mEngine.computePipeline().create(
			computeShaderFilepath, descriptorSetLayouts);
	}
	else
	{
		throw std::runtime_error("A compute pipeline with that name already exists!");
	}
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

void Renderer::resize()
{
	mEngine.resize(mCommandPool, mRenderPass, mSwapchain, mDepthTexture);
}

void Renderer::updateModels(std::unordered_map<uint32_t, ModelData>& modelData)
{
	auto renderSystem = mEngine.ecs().getSystem<RenderSystem>();
	renderSystem->updateModels(modelData);
}

void Renderer::drawFrame()
{
	mEngine.ecs().updateSystem<RenderSystem>(0);
	auto renderSystem = mEngine.ecs().getSystem<RenderSystem>();
	const auto& instances = renderSystem->getInstances();
	const auto& drawCommands = renderSystem->getDrawCommands();

	if (!instances.empty())
	{
		mEngine.updateBuffer<GPUInstance>(mBuffers.at("instances"), mCommandPool, instances);
	}

	if (!drawCommands.empty())
	{
		mEngine.updateBuffer<IndirectBuffer::DrawCommand>(mIndirectBuffer, mCommandPool, drawCommands);
	}

	bool isResized = false;
	mEngine.drawFrame(mCommandPool, mRenderPass, mSwapchain, mRenderGraph, mIndirectBuffer, drawCommands, isResized);

	if (isResized)
	{
		mEngine.resize(mCommandPool, mRenderPass, mSwapchain, mDepthTexture);
	}
}

void Renderer::cleanup()
{
	mEngine.wait();

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

	if (mIndirectBuffer) mEngine.destroy<Buffer2>(*mIndirectBuffer);

	for (const auto& [name, buffer] : mBuffers)
	{
		if (buffer) mEngine.destroy<Buffer2>(*buffer);
	}

	for (const auto& [name, texture] : mTextures)
	{
		if (texture) mEngine.destroy<Texture2>(*texture);
	}

	for (const auto& [name, sampler] : mSamplers)
	{
		if (sampler) mEngine.destroy<Sampler2>(*sampler);
	}

	if (mDescriptorPool) mEngine.destroy<DescriptorPool>(*mDescriptorPool);

	for (const auto& [name, descriptorSetLayout] : mDescriptorSetLayouts)
	{
		if (descriptorSetLayout) mEngine.destroy<DescriptorSetLayout>(*descriptorSetLayout);
	}

	if (mCommandPool) mEngine.destroy<CommandPool>(*mCommandPool);
}

uint32_t Renderer::getWidth() const
{
	return mSwapchain->getExtent().width;
}

uint32_t Renderer::getHeight() const
{
	return mSwapchain->getExtent().height;
}
