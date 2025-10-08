#pragma once

#include "../HandleManager/Handle.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	struct Attachment
	{
		VkAttachmentDescription mDesc{};
		VkAttachmentReference mRef{};
	};

	class RenderPass : public Handle<VkRenderPass>
	{
	public:
		RenderPass(
			VkPhysicalDevice physicalDevice,
			VkFormat colorFormat,
			VkFormat depthFormat);

		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

	protected:
		std::vector<Attachment> mAttachments;
		VkSubpassDescription mSubPassDesc{};
		VkSubpassDependency mSubPassDependency{};
		VkRenderPassCreateInfo mRenderPassInfo{};

	};

}