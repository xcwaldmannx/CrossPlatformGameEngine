#pragma once

#include "../QueueFamily/QueueFamilyManager.h"

#include "../RenderPass/RenderPassManager.h"

#include <vector>

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <vulkan/vulkan.h>

namespace vulkan {

	struct SwapchainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class SwapchainManager {
	public:
		VkResult create(
			GLFWwindow* window,
			const QueueFamilyManager& queueFamilyManager,
			const VkPhysicalDevice& physicalDevice,
			const VkDevice& logicalDevice,
			const VkSurfaceKHR& surface);

		VkResult recreate(GLFWwindow* window,
			const QueueFamilyManager& queueFamilyManager,
			const VkPhysicalDevice& physicalDevice,
			const VkDevice& logicalDevice,
			const VkSurfaceKHR& surface,
			const RenderPassManager& renderpass);

		void destroy(const VkDevice& logicalDevice);

		const VkSwapchainKHR& getSwapchain() const;

		const VkImage& getImage(size_t index) const;
		const std::vector<VkImage>& getImages() const;

		const VkFormat& getFormat() const;
		const VkExtent2D& getExtent() const;

		const VkImageView& getImageView(size_t index) const;
		const std::vector<VkImageView>& getImageViews() const;

		const VkFramebuffer& getFrameBuffer(size_t index) const;
		const std::vector<VkFramebuffer>& getFrameBuffers() const;

		SwapchainSupportDetails querySwapchainSupport(const VkPhysicalDevice& physicalDevice,
			const VkSurfaceKHR& surface) const;

		VkResult createImageViews(const VkDevice& logicalDevice);
		VkResult createFrameBuffers(const VkDevice& logicalDevice, const RenderPassManager& renderpass);
		void destroyFrameBuffers(const VkDevice& logicalDevice);

	private:
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		VkSwapchainKHR mVkSwapchain = nullptr;
		std::vector<VkImage> mSwapchainImages;
		VkFormat mVkSwapchainImageFormat;
		VkExtent2D mVkSwapchainExtent;
		std::vector<VkImageView> mSwapchainImageViews;
		std::vector<VkFramebuffer> mSwapchainFrameBuffers;

		friend class RenderPassManager;
		friend class GraphicsPipelineManager;
	};

}
