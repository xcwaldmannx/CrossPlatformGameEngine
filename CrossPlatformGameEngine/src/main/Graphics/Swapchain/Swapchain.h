#pragma once

#include "../Handle/Handle.h"

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
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkSurfaceKHR surface,
			uint32_t graphicsFamily,
			uint32_t presentFamily);

	public:
		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

		//void recreate(VkDevice device);

		const VkExtent2D& getExtent();
		uint32_t getImageCount() const;
		const std::vector<VkFramebuffer>& getFramebuffers();

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

		void createFrameBuffers(
			VkDevice device,
			VkRenderPass renderPass,
			VkImageView depthImageView);

		void destroyFrameBuffers(VkDevice device);

		void createImageViews(VkDevice device);

		void destroyImageViews(VkDevice device);

	private:
		std::vector<VkImage> mImages;
		std::vector<VkImageView> mImageViews;
		std::vector<VkFramebuffer> mFrameBuffers;

		uint32_t mImageCount;
		VkFormat mImageFormat{};
		VkExtent2D mExtent{};

		friend class SwapchainFactory;
	};

}