#include "MyGame.h"

#include "Utility/ImageLoader/ImageLoader.h"

#include "Graphics/Ecs/Components/TransformComponent.h"
#include "Graphics/Ecs/Components/ModelComponent.h"

#include "Graphics/Ecs/Systems/RenderSystem.h"

MyGame::MyGame(WindowManager& windowManager) :
	mEngine(windowManager),
	mRenderer(windowManager, mEngine)
{
	std::vector<float> vertices;
	vertices.resize(1);
	mRenderer.createVertexBuffer("vertices", vertices);

	std::vector<uint32_t> indices;
	indices.resize(1);
	mRenderer.createIndexBuffer("indices", indices);

	mRenderer.createUniformBuffer<GPUCamera>("camera");

	std::vector<float> transforms;
	transforms.resize(1);
	mRenderer.createStorageBuffer<float>("transforms", transforms);

	std::vector<GPUInstance> instances;
	instances.resize(1);
	mRenderer.createStorageBuffer<GPUInstance>("instances", instances);

	mRenderer.createImageTexture("textures", { 0 }, 1, 1, 1);

	std::vector<ascen::Renderer::ResourceLayout> resources =
	{
		{ "camera", 0x00, sizeof(GPUCamera), ascen::Renderer::ResourceType::UBO_DYNAMIC, ascen::Renderer::ResourceStage::VERTEX },
		{ "transforms", 0x01, VK_WHOLE_SIZE, ascen::Renderer::ResourceType::SSBO, ascen::Renderer::ResourceStage::VERTEX },
		{ "instances", 0x02, VK_WHOLE_SIZE, ascen::Renderer::ResourceType::SSBO, ascen::Renderer::ResourceStage::VERTEX },
		{ "textures", 0x10, sizeof(GPUCamera), ascen::Renderer::ResourceType::IMAGE, ascen::Renderer::ResourceStage::PIXEL }
	};

	mRenderer.createResourceLayouts(resources);

	// mRenderer.createGraphicsPipeline<Vertex>("graphics", "src/shaders/GPUDrivenVS.spv", "src/shaders/GPUDrivenPS.spv");

	//createCommandPool();

	//createBuffers();
	//createTextures();
	//createSamplers();

	//createDescriptor();
	//createSwapchain();
	//createRenderPass();
	//createPipelines();

	//mEngine.resize(mCommandPool, mRenderPass, mSwapchain, mDepthTexture);

	mEngine.ecs().registerComponent<TransformComponent>();
	mEngine.ecs().registerComponent<ModelComponent>();

	std::unordered_map<uint32_t, ModelData> modelData;

	auto readSig = mEngine.ecs().getSignature<TransformComponent, ModelComponent>();
	auto writeSig = mEngine.ecs().getSignature<ModelComponent>();
	mEngine.ecs().registerSystem<RenderSystem>(readSig, writeSig, &modelData);
}

void MyGame::run(float delta)
{
	if (mWindowManager.isResized())
	{
		mRenderer.resize();
		mWindowManager.setResized(false);
	}
}

void MyGame::createCommandPool()
{
	mCommandPool = mEngine.commandPool().create(mEngine.getGraphicsFamily());
}

void MyGame::createDescriptor()
{
	//const std::vector<ascen::DescriptorPool::Size> poolSizes =
	//{
	//	ascen::DescriptorPool::Size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1),
	//	ascen::DescriptorPool::Size(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2),
	//	ascen::DescriptorPool::Size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1),
	//};

	//mDescriptorPool = mEngine.descriptor().createPool(poolSizes);

	//auto bindingCamera     = mEngine.descriptor().createBinding(
	//	0x00, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT);
	//auto bindingTransforms = mEngine.descriptor().createBinding(
	//	0x01, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	//auto bindingInstances  = mEngine.descriptor().createBinding(
	//	0x02, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	//auto bindingTextures   = mEngine.descriptor().createBinding(
	//	0x10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	//const std::vector<ascen::DescriptorSetLayout::Binding> bindings =
	//{
	//	bindingCamera,
	//	bindingTransforms,
	//	bindingInstances,
	//	bindingTextures,
	//};

	//mDescriptorSetLayout = mEngine.descriptor().createLayout(bindings);

	//auto writeCamera = mEngine.descriptor().createBufferWrite(
	//	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, mCameraBuffer->getBuffer(), 0, sizeof(GPUCamera), 0x00);
	//auto writeTransforms = mEngine.descriptor().createBufferWrite(
	//	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, mTransformBuffer->getBuffer(), 0, VK_WHOLE_SIZE, 0x01);
	//auto writeInstances = mEngine.descriptor().createBufferWrite(
	//	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, mInstanceBuffer->getBuffer(), 0, VK_WHOLE_SIZE, 0x02);
	//auto writeTextures = mEngine.descriptor().createImageWrite(
	//	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, mTexture->getView(), mSampler->getSampler(), 0x10);

	//std::vector<ascen::DescriptorSet::Write> writes =
	//{
	//	writeCamera,
	//	writeTransforms,
	//	writeInstances,
	//	writeTextures,
	//};

	//mDescriptorSet = mEngine.descriptor().createSet(mDescriptorPool, mDescriptorSetLayout, writes);
}

void MyGame::createSwapchain()
{
	mSwapchain = mEngine.swapchain().create(
		mWindowManager.getWindow(), mEngine.getGraphicsFamily(), mEngine.getPresentFamily());
}

void MyGame::createRenderPass()
{
	mRenderPass = mEngine.renderPass().create(VK_FORMAT_R8G8B8A8_SRGB);
}

void MyGame::createPipelines()
{
	const std::string vertex = "src/shaders/GPUDrivenVS.spv";
	const std::string pixel = "src/shaders/GPUDrivenPS.spv";

	mGraphicsPipeline = mEngine.graphicsPipeline().create<MyGame::Vertex>(
		vertex, pixel, mDescriptorSetLayout, mSwapchain, mRenderPass);

	const std::string compute = "src/shaders/GPUDrivenCS.spv";

	mComputePipeline = mEngine.computePipeline().create(compute, mDescriptorSetLayout);
}

void MyGame::destroy()
{

	mRenderer.cleanup();
	mEngine.cleanup();

	/*
	mEngine.destroy<ascen::ComputePipeline_I>(*mComputePipeline);
	mEngine.destroy<ascen::GraphicsPipeline_I>(*mGraphicsPipeline);
	mEngine.destroy<ascen::RenderPass>(*mRenderPass);
	mEngine.destroy<ascen::Swapchain>(*mSwapchain);

	mEngine.destroySampler(mSampler);
	mEngine.destroyTexture(mTexture);

	mEngine.destroyBuffer(mIndirectBuffer);
	mEngine.destroyBuffer(mInstanceBuffer);
	mEngine.destroyBuffer(mTransformBuffer);
	mEngine.destroyBuffer(mIndexBuffer);
	mEngine.destroyBuffer(mVertexBuffer);
	mEngine.destroyBuffer(mCameraBuffer);

	mEngine.destroy<ascen::DescriptorSetLayout>(*mDescriptorSetLayout);
	mEngine.destroy<ascen::DescriptorPool>(*mDescriptorPool);
	mEngine.destroy<ascen::CommandPool>(*mCommandPool);
	*/
}

void MyGame::createBuffers()
{
	/*
	mCameraBuffer = mEngine.buffer().createUniform<GPUCamera>(2);

	std::vector<float> vertices;
	vertices.resize(300);
	mVertexBuffer = mEngine.buffer().createVertex(0, mCommandPool, vertices);

	std::vector<uint32_t> indices;
	indices.resize(300);
	mIndexBuffer = mEngine.buffer().createIndex<uint32_t>(0, mCommandPool, indices);

	std::vector<float> transforms;
	transforms.resize(10);
	mTransformBuffer = mEngine.buffer().createStorage<float>(0, mCommandPool, transforms);

	std::vector<GPUInstance> dummyInstanceData{};
	dummyInstanceData.resize(1000);
	mInstanceBuffer = mEngine.buffer().createStorage<GPUInstance>(0, mCommandPool, dummyInstanceData);

	std::vector<VkDrawIndexedIndirectCommand> dummyIndirectData{};
	dummyIndirectData.resize(1000);
	mIndirectBuffer = mEngine.buffer().createIndirect(0, mCommandPool, dummyIndirectData);
	*/
}

void MyGame::createTextures()
{
	ImageLoader il;

	std::vector<const char*> mTextureFilepaths =
	{
		"res/textures/testimg1.png",
		"res/textures/testimg2.png",
		"res/textures/testimg3.png",
	};

	std::vector<unsigned char> pixels;
	uint32_t width = 1024;
	uint32_t height = 1024;
	uint32_t channels = 4;

	pixels.reserve(width * height * channels * mTextureFilepaths.size());

	for (const auto& filepath : mTextureFilepaths)
	{
		RawImage raw;
		il.loadImage(filepath, &raw);
		pixels.insert(pixels.end(), raw.mPixels.begin(), raw.mPixels.end());
	}

	// mTexture = mEngine.createTexture(mCommandPool, pixels, width, height, mTextureFilepaths.size());
}

void MyGame::createSamplers()
{
	mSampler = mEngine.createSampler();
}
