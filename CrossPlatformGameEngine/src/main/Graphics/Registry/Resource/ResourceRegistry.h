#pragma once

#include "../Registry_I.h"

#include "../../VulkanContext.h"
#include "../../RenderContext.h"

#include "../../Resource/Buffer/Buffer2.h"
#include "../../Resource/Texture/Texture2.h"

#include <unordered_map>
#include <vector>

namespace ascen
{

	enum class BufferType
	{
		NONE,
		VERTEX,
		INDEX,
		UNIFORM,
		STORAGE
	};

	struct BufferEntry
	{
		std::string mName;
		BufferType mType = BufferType::NONE;
		uint32_t mCapacity = 0;
		uint32_t mStride = 0;
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

		void updateBuffer(
			const std::string& name,
			const void* items,
			uint32_t itemCount,
			uint32_t itemSize);

		void updateTexture(
			const std::string& name,
			const std::vector<unsigned char>& pixels);

		void cleanup();

		// const MaterialPtr& getMaterial();

	private:
		bool isRegistered(const std::string& name) const;
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
