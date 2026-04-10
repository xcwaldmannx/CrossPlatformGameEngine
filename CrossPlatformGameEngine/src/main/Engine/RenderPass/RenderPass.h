#pragma once

#include "../Handle/Handle.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class RenderPass : public Handle<VkRenderPass>
	{
	public:
		struct Attachment
		{
			VkAttachmentDescription mDesc{};
			VkAttachmentReference mRef{};
		};

	private:
		RenderPass(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkFormat colorFormat,
			VkFormat depthFormat);

	public:
		void destroy(VkDevice device) override;

	protected:
		std::vector<Attachment> mAttachments;
		std::vector<VkAttachmentDescription> mAttachmentDescriptions;
		VkSubpassDescription mSubPassDesc{};
		VkSubpassDependency mSubPassDependency{};
		VkRenderPassCreateInfo mRenderPassInfo{};

		friend class RenderPassFactory;
	};

}