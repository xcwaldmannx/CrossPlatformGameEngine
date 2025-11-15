#pragma once

#include "WindowManager/WindowManager.h"
#include "Graphics/Engine.h"

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MyGame
{
public:
	struct Vertex : public ascen::Vertex_I
	{
		glm::vec3 mPosition;
		glm::vec3 mNormal;
		glm::vec2 mTexCoord;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
			attributeDescriptions.resize(3);

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, mPosition);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, mNormal);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, mTexCoord);

			return attributeDescriptions;
		}
	};

	struct GPUCamera
	{
		glm::mat4 mView;
		glm::mat4 mProj;
	};

	struct GPUInstance
	{
		glm::mat4 mTransform = glm::mat4(1.0);
	};

	MyGame(WindowManager* windowManager);

	void run(float delta);

	void destroy();

private:
	void createCommandPool();

	void createBuffers();
	void createTextures();
	void createSamplers();

	void createDescriptor();
	void createSwapchain();
	void createRenderPass();
	void createPipelines();

private:
	WindowManager mWindowManager;
	ascen::Engine mEngine;

	// buffers
	std::shared_ptr<ascen::Buffer> mCameraBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mVertexBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mIndexBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mTransformBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mInstanceBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mIndirectBuffer = nullptr;

	// textures
	std::shared_ptr<ascen::Texture> mDepthTexture = nullptr;
	std::shared_ptr<ascen::Texture> mTexture = nullptr;

	// samplers
	std::shared_ptr<ascen::Sampler> mSampler = nullptr;

	// command pools
	ascen::CommandPoolPtr mCommandPool = nullptr;

	// descriptors
	ascen::DescriptorPoolPtr mDescriptorPool = nullptr;
	ascen::DescriptorSetLayoutPtr mDescriptorSetLayout = nullptr;
	ascen::DescriptorSetPtr mDescriptorSet = nullptr;

	// graphics pipelines
	ascen::SwapchainPtr mSwapchain = nullptr;
	ascen::RenderPassPtr mRenderPass = nullptr;
	ascen::GraphicsPipelinePtr mGraphicsPipeline = nullptr;

	// compute pipelines
	ascen::ComputePipelinePtr mComputePipeline = nullptr;

};