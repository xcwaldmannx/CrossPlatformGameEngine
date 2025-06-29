#include "TextureHandler.h"

#include "../ResourceCommonFunctions.h"
#include "../ResourceCommonTypes.h"

#include <vulkan/vulkan.h>

namespace ascen {

	void TextureHandler::createSampler(
		PhysicalDevice& physicalDevice,
		LogicalDevice& logicalDevice,
		VkSampler& sampler) {

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice.mDevice, &properties);
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(logicalDevice.mDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	void createTexture(
		const TextureCreateInfo& info,
		const RawImage& rawImage,
		Texture* texture) {

		createImage(*info.mImageCreateInfo, rawImage, &texture->mImage);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = texture->mImage.mImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = (VkFormat) info.mImageCreateInfo->mFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(
			info.mImageCreateInfo->mLogicalDevice->mDevice, &viewInfo, nullptr, &texture->mView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		TextureHandler::createSampler(
			*info.mImageCreateInfo->mPhysicalDevice,
			*info.mImageCreateInfo->mLogicalDevice,
			texture->mSampler);
	}

	void createTexture(
		const TextureCreateInfo& info,
		const char* filepath,
		Texture* texture) {

		RawImage rawImage;
		ImageLoader::loadImage(filepath, &rawImage);

		createTexture(info, rawImage, texture);

		ImageLoader::unloadImage(rawImage);

	}

	void destroyTexture(LogicalDevice& logicalDevice, Texture& texture) {
		vkDestroySampler(logicalDevice.mDevice, texture.mSampler, nullptr);
		vkDestroyImageView(logicalDevice.mDevice, texture.mView, nullptr);
		destroyImage(logicalDevice, texture.mImage);
	}

	void createDepthTexture(const DepthTextureCreateInfo& info, Texture* texture) {

		createDepthImage(*info.mDepthImageCreateInfo, &texture->mImage);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = texture->mImage.mImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = (VkFormat) info.mDepthImageCreateInfo->mImageCreateInfo->mFormat;
		viewInfo.subresourceRange.aspectMask = info.mDepthImageCreateInfo->mImageCreateInfo->mFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(
			info.mDepthImageCreateInfo->mImageCreateInfo->mLogicalDevice->mDevice, &viewInfo, nullptr, &texture->mView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
	}

	void destroyDepthTexture(LogicalDevice& logicalDevice, Texture& texture) {
		vkDestroyImageView(logicalDevice.mDevice, texture.mView, nullptr);
		destroyImage(logicalDevice, texture.mImage);
	}

	void createTextureArray(
		const TextureArrayCreateInfo& info,
		const std::vector<RawImage>& rawImages,
		TextureArray* textureArray) {

		createImages(*info.mImageCreateInfo, rawImages, &textureArray->mImage);

		size_t textureCount = rawImages.size();

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = textureArray->mImage.mImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		viewInfo.format = static_cast<VkFormat>(info.mImageCreateInfo->mFormat);
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = static_cast<uint32_t>(textureCount);

		if (vkCreateImageView(
			info.mImageCreateInfo->mLogicalDevice->mDevice,
			&viewInfo,
			nullptr,
			&textureArray->mView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create 2D array texture image view!");
		}

		// TODO: create samplers separately since they aren't required per image
		TextureHandler::createSampler(
			*info.mImageCreateInfo->mPhysicalDevice,
			*info.mImageCreateInfo->mLogicalDevice,
			textureArray->mSampler);
	}

	void destroyTextureArray(LogicalDevice& logicalDevice, TextureArray& textureArray) {
		vkDestroySampler(logicalDevice.mDevice, textureArray.mSampler, nullptr);
		vkDestroyImageView(logicalDevice.mDevice, textureArray.mView, nullptr);
		destroyImage(logicalDevice, textureArray.mImage);
	}

	VkFormat findDepthFormat(const PhysicalDevice& physicalDevice) {
		std::vector<VkFormat> candidates = {
			VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
		};
		VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
		VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice.mDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR &&
				(props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
				(props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("Failed to find suitable depth format!");

	}

}