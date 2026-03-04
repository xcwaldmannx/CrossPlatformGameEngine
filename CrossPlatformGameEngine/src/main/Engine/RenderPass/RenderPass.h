#pragma once

#include "../Handle/Handle.h"

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
	private:
		RenderPass(
			VkPhysicalDevice physicalDevice,
			VkFormat colorFormat,
			VkFormat depthFormat);

	public:
		void create(VkDevice device) override;
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