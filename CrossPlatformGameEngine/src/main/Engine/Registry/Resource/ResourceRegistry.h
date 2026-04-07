#pragma once

#include <span>

#include "../Registry_I.h"
#include "../../Core/Types.h"

#include "../../Resource/Buffer/Buffer.h"

#include <string>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>


namespace ascen
{

	class VulkanContext;
	class RenderContext;

	class BufferFactory;
	class TextureFactory;
	class SamplerFactory;

	struct BufferEntry
	{
		std::string mName;
		uint32_t mCapacity = 1; // default must be > 0
		uint32_t mStride = 1;   // default must be > 0
		BufferUsageFlags mUsageFlags = 0;
		BufferMemoryFlags mMemoryFlags = 0;
	};

	enum class TextureType
	{
		NONE,
		IMAGE,
		DEPTH,
	};

	struct TextureEntry
	{
		std::string mName;
		TextureType mType = TextureType::NONE;
		uint32_t mWidth = 0;
		uint32_t mHeight = 0;
		uint32_t mLayers = 0;
	};

	struct SamplerEntry
	{
		std::string mName;
	};

	struct MaterialEntry
	{

	};

	class ResourceRegistry : public Registry_I
	{
	public:
		ResourceRegistry(
			VulkanContext& vulkanContext,
			RenderContext& renderContext);

		void registerBuffer(BufferEntry entry);
		void registerTexture(TextureEntry entry);
		void registerSampler(SamplerEntry entry);
		// void registerMaterial(MaterialEntry&& entry);

		void reconstruct() override;

		template<typename T>
		std::span<T> getMappedBuffer(const std::string& name)
		{
			auto it = mBuffers.find(name);
			if (it == mBuffers.end())
			{
				throw std::runtime_error("Mapped buffer does not exist!");
			}

			const BufferPtr& buf = it->second;

			void* mapped = buf->getMappedMemory();

			auto* typed = reinterpret_cast<T*>(static_cast<uint8_t*>(mapped));

			return std::span<T>(typed, buf->getItemCount());
		}

		void uploadBuffer(
			const std::string& name,
			const void* items,
			uint32_t itemCount,
			uint32_t itemSize,
			uint32_t offset = 0) const;

		template<typename T>
		void downloadBuffer(const std::string& name, T* data, const size_t count)
		{
			const auto it = mBuffers.find(name);
			if (it == mBuffers.end())
			{
				throw std::runtime_error("Buffer does not exist!");
			}

			const BufferPtr& buf = it->second;
			buf->download(mPhysicalDevice, mDevice, mGraphicsQueue, mCommandPool, data, count, sizeof(T));
		}

		void uploadTexture(
			const std::string& name,
			const std::vector<unsigned char>& pixels) const;

		void cleanup();

		// const MaterialPtr& getMaterial();

	private:
		bool isRegistered(const std::string& name) const;
		bool pushConstantExists(const std::string& name) const;
		bool bufferExists(const std::string& name) const;
		bool textureExists(const std::string& name) const;
		bool samplerExists(const std::string& name) const;

	private:
		const VkPhysicalDevice mPhysicalDevice;
		const VkDevice mDevice;
		const VkQueue mGraphicsQueue;
		const CommandPoolPtr& mCommandPool;

		const BufferFactory& mBufferFactory;
		const TextureFactory& mTextureFactory;
		const SamplerFactory& mSamplerFactory;


		std::vector<std::string> mRegisteredNames;

		std::vector<BufferEntry> mBufferEntries;
		std::vector<TextureEntry> mTextureEntries;
		std::vector<SamplerEntry> mSamplerEntries;
		// std::vector<MaterialEntry> mMaterialEntries;

		std::unordered_map<std::string, BufferPtr> mBuffers;
		std::unordered_map<std::string, TexturePtr> mTextures;
		std::unordered_map<std::string, SamplerPtr> mSamplers;
		// std::unordered_map<std::string, MaterialPtr> mMaterials;

		friend class ResourceRegistryBackend;
	};

}
