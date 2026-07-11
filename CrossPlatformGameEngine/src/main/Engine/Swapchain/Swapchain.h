#pragma once

#include "../Handle/Handle.h"
#include "../Device/Physical/PhysicalDevice.h"
#include "../Resource/Image/Image.h"

#include <vector>

#include <vulkan/vulkan.h>

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

namespace ascen
{

	struct SwapchainSupportDetails
	{
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
		VkSurfaceCapabilitiesKHR mCapabilities{};
	};

	class Swapchain : public Handle<VkSwapchainKHR>
	{
	private:
		Swapchain(
			GLFWwindow* window,
			const VkPhysicalDevice physicalDevice,
			const VkDevice device,
			const VkSurfaceKHR surface,
			const uint32_t graphicsFamily,
			const uint32_t presentFamily);

	public:
		void destroy(VkDevice device) override;

		//void recreate(VkDevice device);

		const VkExtent2D& getExtent() const;
		uint32_t getImageCount() const;
		const std::vector<VkImage>& getImages() const;
		VkFormat getImageFormat() const;

		VkFramebuffer_T* getFrameBuffer(uint32_t imageIndex) const;

		static SwapchainSupportDetails querySwapchainSupport(
			VkPhysicalDevice physicalDevice,
			VkSurfaceKHR surface);

		static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR>& availableFormats);

		static VkPresentModeKHR chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes);

		static VkExtent2D chooseSwapExtent(
			GLFWwindow* window,
			const VkSurfaceCapabilitiesKHR& capabilities);

		void setDepthTexture(const TexturePtr& texture);

		void createFrameBuffers(
			VkDevice device,
			VkRenderPass renderPass);

		void destroyFrameBuffers(VkDevice device) const;

		void createImageViews(VkDevice device);

		void destroyImageViews(VkDevice device) const;

	private:
		const VkPhysicalDevice mPhysicalDevice;
		const VkDevice mDevice;

		std::vector<VkImage> mImages;
		std::vector<VkImageView> mImageViews;

		TexturePtr mDepthTexture = nullptr;

		std::vector<VkFramebuffer> mFrameBuffers;
		// uint32_t mCurrentFrameIndex = 0;

		uint32_t mImageCount;
		VkFormat mImageFormat{};
		VkExtent2D mExtent{};

		friend class SwapchainFactory;
	};

}