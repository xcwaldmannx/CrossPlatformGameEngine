#include "TestGraphicsPipeline.h"

TestGraphicsPipeline::TestGraphicsPipeline(
	const WindowManager& windowManager,
	const std::string vertexShaderFilepath,
	const std::string pixelShaderFilepath,
	const std::vector<float> vertices,
	const std::vector<uint32_t> indices,
	const std::vector<float> transforms) :
	GraphicsPipeline<ascen::TextureVertex>(windowManager, vertexShaderFilepath, pixelShaderFilepath),
	mVertices(vertices),
	mIndices(indices), 
	mTransforms(transforms) {}

void TestGraphicsPipeline::createShaderResources()
{
	createBuffers();
	createTextures();
	createSamplers();
}

void TestGraphicsPipeline::destroyShaderResources()
{
	ascen::Buffer::destroy(mDevice, *mUniformBuffer);
	ascen::Buffer::destroy(mDevice, *mVertexBuffer);
	ascen::Buffer::destroy(mDevice, *mIndexBuffer);
	ascen::Buffer::destroy(mDevice, *mInstanceBuffer);
	ascen::Buffer::destroy(mDevice, *mIndirectBuffer);
	ascen::Texture::destroy(mDevice, *mTexture);
	ascen::Sampler::destroy(mDevice, *mSampler);
}

void TestGraphicsPipeline::createDescriptorResources()
{
	std::vector<ascen::Descriptor::PoolSize> poolSizes =
	{
		ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1),
		ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1),
		ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1),
	};

	mDescriptorPool = ascen::Descriptor::createPool(mDevice, poolSizes);

	std::vector<VkDescriptorSetLayoutBinding> bindings =
	{
		ascen::Descriptor::createBinding(
			0,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			VK_SHADER_STAGE_VERTEX_BIT),

		ascen::Descriptor::createBinding(
			1,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_VERTEX_BIT),

		ascen::Descriptor::createBinding(
			2,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_SHADER_STAGE_FRAGMENT_BIT),
	};

	mDescriptorSetLayout = ascen::Descriptor::createLayout(mDevice, bindings);

	std::vector<VkDescriptorSetLayout> layouts =
	{
		mDescriptorSetLayout
	};

	mDescriptorSet = ascen::Descriptor::createSet(mDevice, mDescriptorPool, layouts);

	auto writeUniformBuffer = ascen::Descriptor::createBufferWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		mUniformBuffer->getBuffer(),
		0,
		sizeof(TestGPUCameraData),
		0);

	auto writeInstanceBuffer = ascen::Descriptor::createBufferWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		mInstanceBuffer->getBuffer(),
		0,
		VK_WHOLE_SIZE,
		1);

	auto writeTextureBuffer = ascen::Descriptor::createImageWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		mTexture->getView(),
		mSampler->getSampler(),
		2);

	std::vector<ascen::DescriptorWrite> writes =
	{
		writeUniformBuffer,
		writeInstanceBuffer,
		writeTextureBuffer,
	};

	ascen::Descriptor::updateSet(mDevice, writes);
}

void TestGraphicsPipeline::destroyDescriptorResources()
{
	ascen::Descriptor::destroy(mDevice, mDescriptorSetLayout, mDescriptorPool);
}

void TestGraphicsPipeline::record(uint32_t imageIndex)
{
	updateUBO(imageIndex);

	mCommandPool->record(
		mPhysicalDevice,
		mCurrentFrame,
		imageIndex,
		mDescriptorSet,
		mVertexBuffer->getBuffer(),
		mIndexBuffer->getBuffer(),
		mIndirectBuffer->getBuffer(),
		mRenderPass,
		mSwapchain,
		mPipeline,
		mDrawCommands);
}

void TestGraphicsPipeline::updateInstances(const std::vector<GPUInstance>& instances)
{
	ascen::Buffer::updateStorageBuffer<GPUInstance>(
		mPhysicalDevice,
		mDevice,
		ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value()),
		mCommandPool,
		*mInstanceBuffer,
		instances);
}

void TestGraphicsPipeline::updateDrawCommands(const std::vector<VkDrawIndexedIndirectCommand>& drawCommands)
{
	ascen::Buffer::updateStorageBuffer<VkDrawIndexedIndirectCommand>(
		mPhysicalDevice,
		mDevice,
		ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value()),
		mCommandPool,
		*mIndirectBuffer,
		drawCommands);
}

void TestGraphicsPipeline::createBuffers()
{
	mUniformBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createUniformBuffer<TestGPUCameraData>(
			mPhysicalDevice,
			mDevice,
			2
		)
	);

	VkQueue graphicsQueue = ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value());

	mVertexBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createVertexBuffer(
			mPhysicalDevice,
			mDevice,
			graphicsQueue,
			mCommandPool,
			mVertices
		)
	);

	mIndexBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createIndexBuffer<uint32_t>(
			mPhysicalDevice,
			mDevice,
			graphicsQueue,
			mCommandPool,
			mIndices
		)
	);

	std::vector<GPUInstance> dummyInstanceData{};
	dummyInstanceData.resize(1000);

	mInstanceBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createStorageBuffer<GPUInstance>(
			mPhysicalDevice,
			mDevice,
			graphicsQueue,
			mCommandPool,
			dummyInstanceData
		)
	);

	std::vector<VkDrawIndexedIndirectCommand> dummyIndirectData{};
	dummyIndirectData.resize(1000);

	mIndirectBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createIndirectBuffer(
			mPhysicalDevice,
			mDevice,
			graphicsQueue,
			mCommandPool,
			dummyIndirectData
		)
	);
}

void TestGraphicsPipeline::createTextures()
{
	ImageLoader il;

	std::vector<const char*> mTextureFilepaths =
	{
		"res/textures/testimg1.png",
		"res/textures/testimg2.png",
		"res/textures/testimg3.png",
	};

	std::vector<unsigned char> pixels;
	uint32_t width    = 1024;
	uint32_t height   = 1024;
	uint32_t channels = 4;

	pixels.reserve(width * height * channels * mTextureFilepaths.size());

	for (const auto& filepath : mTextureFilepaths)
	{
		RawImage raw;
		il.loadImage(filepath, &raw);
		std::cout << "pixel count: " << raw.mPixels.size() << "\n";
		pixels.insert(pixels.end(), raw.mPixels.begin(), raw.mPixels.end());
	}

	mTexture = std::make_shared<ascen::Texture>(
		ascen::Texture::create(
			mPhysicalDevice,
			mDevice,
			ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value()),
			mCommandPool,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_TILING_OPTIMAL,
			pixels,
			width,
			height,
			static_cast<uint32_t>(mTextureFilepaths.size()),
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT
		)
	);
}

void TestGraphicsPipeline::createSamplers()
{
	mSampler = std::make_shared<ascen::Sampler>(
		ascen::Sampler::create(
			mPhysicalDevice,
			mDevice
		)
	);
}

void TestGraphicsPipeline::updateUBO(uint32_t imageIndex)
{
	TestGPUCameraData ubo{};
	ubo.mView = glm::lookAt(glm::vec3(0.0f, -15.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.mProj = glm::perspective(glm::radians(90.0f),
		mSwapchain->getExtent().width / (float)mSwapchain->getExtent().height, 0.01f, 100.0f);
	ubo.mProj[1][1] *= -1;

	uint8_t offset = imageIndex * sizeof(TestGPUCameraData);
	uint8_t* target = reinterpret_cast<uint8_t*>(mUniformBuffer->getMappedMemory());
	memcpy(target + offset, &ubo, sizeof(ubo));
}
