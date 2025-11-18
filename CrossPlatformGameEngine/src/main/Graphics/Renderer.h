#pragma once

#include "../WindowManager/WindowManager.h"
#include "Engine.h"
#include "RenderGraph/RenderGraph.h"

#include <concepts>

#include <unordered_map>

#include <vulkan/vulkan.h>

namespace ascen
{

	class Renderer
	{
	public:
		enum ResourceType
		{
			UBO           = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			UBO_DYNAMIC   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			SSBO          = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			SSBO_DYNAMIC  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
			IMAGE         = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			SAMPLER       = VK_DESCRIPTOR_TYPE_SAMPLER,
			IMAGE_SAMPLER = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		};

		enum ResourceStage
		{
			VERTEX  = VK_SHADER_STAGE_VERTEX_BIT,
			PIXEL   = VK_SHADER_STAGE_FRAGMENT_BIT,
			COMPUTE = VK_SHADER_STAGE_COMPUTE_BIT,
		};

		struct ResourceLayout
		{
			std::string mSetName;
			std::string mResourceName;
			uint32_t mSlot;
			size_t mSize;
			ResourceType mType;
			ResourceStage mStage;
		};

		struct Pass
		{
			std::vector<std::string> mVertexBuffers;
			std::string mIndexBuffer;
			std::vector<std::string> mReadBuffers;
			std::vector<std::string> mWriteBuffers;
			std::vector<std::string> mReadTextures;
			std::vector<std::string> mWriteTextures;
			std::vector<std::string> mDescriptorSets;
			std::string mGraphicsPipeline;
			std::string mComputePipeline;
		};

		Renderer(WindowManager& windowManager, Engine& engine);

		void finalize();

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

		void addResourceLayout(const ResourceLayout& layout)
		{
			mResourceLayouts[layout.mSetName].push_back(layout);
			mDescriptorPoolRequirements[layout.mType]++;
		}

		void createDescriptorSetLayouts()
		{
			std::vector<DescriptorPool::Size> sizes;

			for (auto& [type, count] : mDescriptorPoolRequirements)
			{
				if (count > 0)
				{
					DescriptorPool::Size size{};
					size.type = static_cast<VkDescriptorType>(type);
					size.descriptorCount = count;
					sizes.push_back(size);
				}
			}

			mDescriptorPool = mEngine.descriptor().createPool(sizes);

			for (auto& [set, layouts] : mResourceLayouts)
			{
				if (mDescriptorSetLayouts.find(set) == mDescriptorSetLayouts.end() &&
					mDescriptorSets.find(set) == mDescriptorSets.end())
				{
					std::vector<DescriptorSetLayout::Binding> bindings;
					std::vector<DescriptorSet::Write> writes;

					for (auto& layout : layouts)
					{
						bindings.push_back(mEngine.descriptor().createBinding(
							layout.mSlot,
							static_cast<VkDescriptorType>(layout.mType),
							static_cast<uint32_t>(layout.mStage)));

						switch (layout.mType)
						{
						case ResourceType::IMAGE:
							{
								if (mTextures.find(layout.mResourceName) == mTextures.end())
								{
									throw std::runtime_error("Texture does not exist!");
								}

								const TexturePtr& texture = mTextures[layout.mResourceName];
								writes.push_back(mEngine.descriptor().createImageWrite(
									static_cast<VkDescriptorType>(layout.mType),
									texture->handle(),
									VK_NULL_HANDLE,
									layout.mSlot));
								break;
							}
						case ResourceType::SAMPLER:
							{
								if (mSamplers.find(layout.mResourceName) == mSamplers.end())
								{
									throw std::runtime_error("Sampler does not exist!");
								}

								const SamplerPtr& sampler = mSamplers[layout.mResourceName];
								writes.push_back(mEngine.descriptor().createImageWrite(
									static_cast<VkDescriptorType>(layout.mType),
									VK_NULL_HANDLE,
									sampler->handle(),
									layout.mSlot));
								break;
							}
						case ResourceType::IMAGE_SAMPLER:
							break;
						case ResourceType::UBO:
						case ResourceType::UBO_DYNAMIC:
						case ResourceType::SSBO:
						case ResourceType::SSBO_DYNAMIC:
						default:
							{
								if (mBuffers.find(layout.mResourceName) == mBuffers.end())
								{
									throw std::runtime_error("Buffer does not exist!");
								}

								const BufferPtr& buffer = mBuffers[layout.mResourceName];
								writes.push_back(mEngine.descriptor().createBufferWrite(
									static_cast<VkDescriptorType>(layout.mType),
									buffer->handle(),
									0,
									static_cast<VkDeviceSize>(layout.mSize),
									layout.mSlot));
							}
						}
					}

					mDescriptorSetLayouts.emplace(set, mEngine.descriptor().createSetLayout(bindings));
					mDescriptorSets.emplace(set, mEngine.descriptor().createSet(
						mDescriptorPool, mDescriptorSetLayouts.at(set), writes));
				}
				else
				{
					throw std::runtime_error("A descriptor set layout with that name already exists!");
				}
			}
		}

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

		template<typename T>
		void updateBuffer(const std::string& name, const std::vector<T> data)
		{
			if (mBuffers.find(name) != mBuffers.end())
			{
				mEngine.updateBuffer<T>(mBuffers.at(name), mCommandPool, data);
			}
			else
			{
				throw std::runtime_error("Buffer does not exist!");
			}
		}

		template<typename T>
		void updateUniformBuffer(const std::string& name, const T& data)
		{
			if (mBuffers.find(name) != mBuffers.end())
			{
				mEngine.updateUniformBuffer<T>(mBuffers.at(name), data);
			}
			else
			{
				throw std::runtime_error("Buffer does not exist!");
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

		void createSampler(const std::string& name)
		{
			if (mSamplers.find(name) == mSamplers.end())
			{
				mSamplers[name] = mEngine.sampler().createSampler();
			}
			else
			{
				throw std::runtime_error("A texture with that name already exists!");
			}
		}

		template<std::derived_from<Vertex_I> T>
		void createGraphicsPipeline(
			const std::string& name,
			const std::vector<std::string>& descriptors,
			const std::string& vertexShaderFilepath,
			const std::string& pixelShaderFilepath)
		{
			if (mGraphicsPipelines.find(name) == mGraphicsPipelines.end())
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

				mGraphicsPipelines[name] = mEngine.graphicsPipeline().create<T>(
					vertexShaderFilepath, pixelShaderFilepath, descriptorSetLayouts, mSwapchain, mRenderPass);
			}
			else
			{
				throw std::runtime_error("A graphics pipeline with that name already exists!");
			}
		}

		void createComputePipeline(
			const std::string& name,
			const std::vector<std::string>& descriptors,
			const std::string& computeShaderFilepath);

		void createRenderPass(const std::string& name, const Pass& renderPass);

		void resize();

		void updateModels(std::unordered_map<uint32_t, ModelData>& modelData);

		void drawFrame();

		void cleanup();

		uint32_t getWidth() const;
		uint32_t getHeight() const;

	private:
		WindowManager& mWindowManager;
		Engine& mEngine;
		RenderGraph mRenderGraph;

		const int MAX_FRAMES_IN_FLIGHT = 2;

		CommandPoolPtr mCommandPool = nullptr;
		SwapchainPtr mSwapchain = nullptr;
		RenderPassPtr mRenderPass = nullptr;

		TexturePtr mDepthTexture = nullptr;

		BufferPtr mInstanceBuffer = nullptr;
		BufferPtr mIndirectBuffer = nullptr;

		std::unordered_map<std::string, std::vector<ResourceLayout>> mResourceLayouts;
		std::unordered_map<ResourceType, uint32_t> mDescriptorPoolRequirements;

		DescriptorPoolPtr mDescriptorPool = nullptr;
		std::unordered_map<std::string, DescriptorSetLayoutPtr> mDescriptorSetLayouts;
		std::unordered_map<std::string, DescriptorSetPtr> mDescriptorSets;

		std::unordered_map<std::string, BufferPtr> mBuffers;
		std::unordered_map<std::string, TexturePtr> mTextures;
		std::unordered_map<std::string, SamplerPtr> mSamplers;

		std::unordered_map<std::string, GraphicsPipelinePtr> mGraphicsPipelines;
		std::unordered_map<std::string, ComputePipelinePtr> mComputePipelines;
	};

}
