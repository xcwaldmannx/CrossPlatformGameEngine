#include "MyGraphicsPipeline.h"

MyGraphicsPipeline::MyGraphicsPipeline(
	WindowManager* windowManager,
	const std::string vertexShaderFilepath,
	const std::string pixelShaderFilepath,
	const std::vector<float> vertices,
	const std::vector<uint32_t> indices,
	const std::vector<float> transforms) :
	DefaultGraphicsPipeline<ascen::TextureVertex>(windowManager, vertexShaderFilepath, pixelShaderFilepath),
	mVertices(vertices),
	mIndices(indices), 
	mTransforms(transforms) {}

void MyGraphicsPipeline::createShaderResources()
{
	createBuffers();
	createTextures();
	createSamplers();
}

void MyGraphicsPipeline::destroyShaderResources()
{
	ascen::Buffer::destroy(mDevice, *mCameraBuffer);
	ascen::Buffer::destroy(mDevice, *mVertexBuffer);
	ascen::Buffer::destroy(mDevice, *mIndexBuffer);
	ascen::Buffer::destroy(mDevice, *mTransformBuffer);
	ascen::Buffer::destroy(mDevice, *mInstanceBuffer);
	ascen::Buffer::destroy(mDevice, *mIndirectBuffer);
	ascen::Texture::destroy(mDevice, *mTexture);
	ascen::Sampler::destroy(mDevice, *mSampler);
}

void MyGraphicsPipeline::createDescriptorResources()
{
	std::vector<ascen::Descriptor::PoolSize> poolSizes =
	{
		ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1),
		ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2),
		ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1),
	};

	mDescriptorPool = ascen::Descriptor::createPool(mDevice, poolSizes);

	std::vector<VkDescriptorSetLayoutBinding> bindings =
	{
		// buffer 0x00-0x0F

		// camera ubo
		ascen::Descriptor::createBinding(
			0x00,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			VK_SHADER_STAGE_VERTEX_BIT),

		// transform ssbo
		ascen::Descriptor::createBinding(
			0x01,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_VERTEX_BIT),

		// instance ssbo
		ascen::Descriptor::createBinding(
			0x02,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_VERTEX_BIT),

		// image 0x10-0x1F

		// texture array
		ascen::Descriptor::createBinding(
			0x10,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_SHADER_STAGE_FRAGMENT_BIT),
	};

	mDescriptorSetLayout = ascen::Descriptor::createLayout(mDevice, bindings);

	std::vector<VkDescriptorSetLayout> layouts =
	{
		mDescriptorSetLayout
	};

	mDescriptorSet = ascen::Descriptor::createSet(mDevice, mDescriptorPool, layouts);

	auto writeCameraBuffer = ascen::Descriptor::createBufferWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		mCameraBuffer->getBuffer(),
		0,
		sizeof(GPUCamera),
		0x00);

	auto writeTransformBuffer = ascen::Descriptor::createBufferWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		mTransformBuffer->getBuffer(),
		0,
		VK_WHOLE_SIZE,
		0x01);

	auto writeInstanceBuffer = ascen::Descriptor::createBufferWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		mInstanceBuffer->getBuffer(),
		0,
		VK_WHOLE_SIZE,
		0x02);

	auto writeTextureBuffer = ascen::Descriptor::createImageWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		mTexture->getView(),
		mSampler->getSampler(),
		0x10);

	std::vector<ascen::DescriptorWrite> writes =
	{
		writeCameraBuffer,
		writeTransformBuffer,
		writeInstanceBuffer,
		writeTextureBuffer,
	};

	ascen::Descriptor::updateSet(mDevice, writes);
}

void MyGraphicsPipeline::destroyDescriptorResources()
{
	ascen::Descriptor::destroy(mDevice, mDescriptorSetLayout, mDescriptorPool);
}

void MyGraphicsPipeline::record(uint32_t currentImage)
{
	mCommandPool->record(
		mPhysicalDevice,
		mCurrentFrame,
		currentImage,
		mDescriptorSet,
		mVertexBuffer->getBuffer(),
		mIndexBuffer->getBuffer(),
		mIndirectBuffer->getBuffer(),
		mRenderPass,
		mSwapchain,
		mPipeline,
		mDrawCommands);
}

void MyGraphicsPipeline::updateInstances(const std::vector<GPUInstance>& instances)
{
	ascen::Buffer::updateStorageBuffer<GPUInstance>(
		mPhysicalDevice,
		mDevice,
		ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value()),
		mCommandPool,
		*mInstanceBuffer,
		instances);
}

void MyGraphicsPipeline::updateDrawCommands(const std::vector<VkDrawIndexedIndirectCommand>& drawCommands)
{
	ascen::Buffer::updateStorageBuffer<VkDrawIndexedIndirectCommand>(
		mPhysicalDevice,
		mDevice,
		ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value()),
		mCommandPool,
		*mIndirectBuffer,
		drawCommands);
}

void MyGraphicsPipeline::createBuffers()
{
	mCameraBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createUniformBuffer<GPUCamera>(
			mPhysicalDevice,
			mDevice,
			MAX_FRAMES_IN_FLIGHT
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

	mTransformBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createStorageBuffer<float>(
			mPhysicalDevice,
			mDevice,
			graphicsQueue,
			mCommandPool,
			mTransforms
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

void MyGraphicsPipeline::createTextures()
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

void MyGraphicsPipeline::createSamplers()
{
	mSampler = std::make_shared<ascen::Sampler>(
		ascen::Sampler::create(
			mPhysicalDevice,
			mDevice
		)
	);
}

void MyGraphicsPipeline::updateCamera(glm::mat4& transform)
{
	GPUCamera ubo{};
	ubo.mView = glm::inverse(transform);
	ubo.mProj = glm::perspective(glm::radians(90.0f),
		mSwapchain->getExtent().width / (float)mSwapchain->getExtent().height, 0.01f, 100.0f);
	ubo.mProj[1][1] *= -1;

	size_t offset = mCurrentFrame * sizeof(GPUCamera);
	uint8_t* target = reinterpret_cast<uint8_t*>(mCameraBuffer->getMappedMemory());
	memcpy(target + offset, &ubo, sizeof(ubo));
}
