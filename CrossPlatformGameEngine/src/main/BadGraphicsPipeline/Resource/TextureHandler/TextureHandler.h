#pragma once

#include "../../../Utility/ImageLoader/ImageLoader.h"
#include "../ImageHandler/ImageHandler.h"

#include <vulkan/vulkan.h>

namespace ascen {

	class TextureHandler {

		static void createSampler(
			PhysicalDevice& physicalDevice,
			LogicalDevice& logicalDevice,
			VkSampler& sampler);

		friend void createTexture(
			const TextureCreateInfo& info,
			const RawImage& rawImage,
			Texture* texture);

		friend void createTexture(
			const TextureCreateInfo& info,
			const char* filepath,
			Texture* texture);

		friend void createDepthTexture(const DepthTextureCreateInfo& info, Texture* texture);

		friend void createTextureArray(
			const TextureArrayCreateInfo& info,
			const std::vector<RawImage>& rawImages,
			TextureArray* textureArray);

	};

	void createTexture(
		const TextureCreateInfo& info,
		const RawImage& rawImage,
		Texture* texture);

	void createTexture(
		const TextureCreateInfo& info,
		const char* filepath,
		Texture* texture);

	void destroyTexture(LogicalDevice& logicalDevice, Texture& texture);

	void createDepthTexture(const DepthTextureCreateInfo& info, Texture* texture);
	void destroyDepthTexture(LogicalDevice& logicalDevice, Texture& texture);

	void createTextureArray(
		const TextureArrayCreateInfo& info,
		const std::vector<RawImage>& rawImages,
		TextureArray* textureArray);

	void destroyTextureArray(LogicalDevice& logicalDevice, TextureArray& textureArray);

	VkFormat findDepthFormat(const PhysicalDevice& physicalDevice);
}