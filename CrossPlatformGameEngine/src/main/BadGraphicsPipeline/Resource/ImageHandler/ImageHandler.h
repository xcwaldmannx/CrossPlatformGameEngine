#pragma once

#include "../ResourceCommonFunctions.h"

#include "../../../Utility/ImageLoader/ImageLoader.h"

namespace ascen {

	struct CommandPool;
	struct Image;

	class ImageHandler {

		static void allocateImage(
			const ImageCreateInfo& info,
			Image* image);

		static void transitionImageLayout(
			const ImageCreateInfo& info,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			Image* image);

		static void copyBufferToImage(
			const ImageCreateInfo& info,
			Buffer& buffer,
			Image* image);

		friend void createImage(const ImageCreateInfo& info, const RawImage& rawImage, Image* image);

		friend void createDepthImage(const DepthImageCreateInfo& info, Image* image);

		friend	void createImages(
			const ImageCreateInfo& info,
			std::vector<RawImage> rawImages,
			Image* image);

	};

	void createImage(const ImageCreateInfo& info, const RawImage& rawImage, Image* image);
	void createDepthImage(const DepthImageCreateInfo& info, Image* image);
	void destroyImage(LogicalDevice logicalDevice, Image& image);

	void createImages(
		const ImageCreateInfo& info,
		std::vector<RawImage> rawImages,
		Image* image);

	
}