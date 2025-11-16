#pragma once

#include "../WindowManager/WindowManager.h"
#include "Engine.h"

#include <concepts>

#include <unordered_map>

#include <vulkan/vulkan.h>

namespace ascen
{

	class Renderer
	{
	public:
		enum class ResourceType
		{
			UBO           = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			UBO_DYNAMIC   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			SSBO          = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			SSBO_DYNAMIC  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
			IMAGE         = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			SAMPLER       = VK_DESCRIPTOR_TYPE_SAMPLER,
			IMAGE_SAMPLER = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		};

		enum class ResourceStage
		{
			VERTEX  = VK_SHADER_STAGE_VERTEX_BIT,
			PIXEL   = VK_SHADER_STAGE_FRAGMENT_BIT,
			COMPUTE = VK_SHADER_STAGE_COMPUTE_BIT,
		};

		struct ResourceLayout
		{
			std::string mName;
			uint32_t mSlot;
			size_t mSize;
			ResourceType mType;
			ResourceStage mStage;
		};

		Renderer(WindowManager& windowManager, Engine& engine);

		void createResourceLayouts(std::vector<ResourceLayout>& layouts);

		// leave this here for now
		
		//template<std::derived_from<Vertex_I> T>
		//void createVertexBuffer(const std::string& name, const std::vector<T>& data)
		//{
		//	if (mBuffers.find(name) == mBuffers.end())
		//	{
		//		mBuffers[name] = mEngine.buffer().createVertex<T>(mEngine.getGraphicsFamily(), mCommandPool, data);
		//	}
		//	else
		//	{
		//		throw std::runtime_error("A buffer with that name already exists!");
		//	}
		//}

		void createVertexBuffer(const std::string& name, const std::vector<float>& data)
		{
			if (mBuffers.find(name) == mBuffers.end())
			{
				mBuffers[name] = mEngine.buffer().createVertex(mEngine.getGraphicsFamily(), mCommandPool, data);
			}
			else
			{
				throw std::runtime_error("A buffer with that name already exists!");
			}
		}

		void createIndexBuffer(const std::string& name, const std::vector<uint32_t>& data)
		{
			if (mBuffers.find(name) == mBuffers.end())
			{
				mBuffers[name] = mEngine.buffer().createIndex(mEngine.getGraphicsFamily(), mCommandPool, data);
			}
			else
			{
				throw std::runtime_error("A buffer with that name already exists!");
			}
		}

		template<typename T>
		void createUniformBuffer(const std::string& name)
		{
			if (mBuffers.find(name) == mBuffers.end())
			{
				mBuffers[name] = mEngine.buffer().createUniform<T>(MAX_FRAMES_IN_FLIGHT);
			}
			else
			{
				throw std::runtime_error("A buffer with that name already exists!");
			}
		}

		template<typename T>
		void createStorageBuffer(const std::string& name, const std::vector<T>& data)
		{
			if (mBuffers.find(name) == mBuffers.end())
			{
				mBuffers[name] = mEngine.buffer().createStorage(mEngine.getGraphicsFamily(), mCommandPool, data);
			}
			else
			{
				throw std::runtime_error("A buffer with that name already exists!");
			}
		}

		void createIndirectBuffer(const std::string& name, const std::vector<Buffer::DrawCommand>& data)
		{
			if (mBuffers.find(name) == mBuffers.end())
			{
				mBuffers[name] = mEngine.buffer().createIndirect(mEngine.getGraphicsFamily(), mCommandPool, data);
			}
			else
			{
				throw std::runtime_error("A buffer with that name already exists!");
			}
		}

		void createImageTexture(
			const std::string& name,
			const std::vector<unsigned char>& pixels,
			uint32_t width,
			uint32_t height,
			uint32_t layers)
		{
			if (mTextures.find(name) == mTextures.end())
			{
				mTextures[name] = mEngine.texture().createImage(
					mEngine.getGraphicsFamily(), mCommandPool, pixels, width, height, layers);
			}
			else
			{
				throw std::runtime_error("A texture with that name already exists!");
			}
		}

		template<std::derived_from<Vertex_I> T>
		void createGraphicsPipeline(
			const std::string& name,
			const std::string& vertexShaderFilepath,
			const std::string& pixelShaderFilepath)
		{
			if (mGraphicsPipelines.find(name) == mGraphicsPipelines.end())
			{
				mGraphicsPipelines[name] = mEngine.graphicsPipeline().create<T>(
					vertexShaderFilepath, pixelShaderFilepath, mDescriptorSetLayout, mSwapchain, mRenderPass);
			}
			else
			{
				throw std::runtime_error("A graphics pipeline with that name already exists!");
			}
		}

		void createComputePipeline(
			const std::string& name,
			const std::string& computeShaderFilepath);

		void resize();

		void drawFrame();

		void cleanup();

	private:
		WindowManager& mWindowManager;
		Engine& mEngine;

		const int MAX_FRAMES_IN_FLIGHT = 2;

		CommandPoolPtr mCommandPool = nullptr;
		SwapchainPtr mSwapchain = nullptr;
		RenderPassPtr mRenderPass = nullptr;

		DescriptorPoolPtr mDescriptorPool = nullptr;
		DescriptorSetLayoutPtr mDescriptorSetLayout = nullptr;
		DescriptorSetPtr mDescriptorSet = nullptr;

		TexturePtr mDepthTexture = nullptr;

		std::unordered_map<std::string, BufferPtr> mBuffers;
		std::unordered_map<std::string, TexturePtr> mTextures;
		std::unordered_map<std::string, SamplerPtr> mSamplers;

		std::unordered_map<std::string, GraphicsPipelinePtr> mGraphicsPipelines;
		std::unordered_map<std::string, ComputePipelinePtr> mComputePipelines;
	};

}
