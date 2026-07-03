#pragma once

#include "VulkanContext.h"
#include "RenderContext.h"

#include "Values.h"

#include "../Registry/Vertex/VertexRegistry.h"
#include "../Registry/Resource/ResourceRegistry.h"
#include "../Registry/Descriptor/DescriptorRegistry.h"
#include "../Registry/Pipeline/PipelineRegistry.h"
#include "../Registry/FramePass/FramePassRegistry.h"
#include "../Registry/RenderTarget/RenderTargetRegistry.h"

#include "Renderer.h"

#include "../../EcsSystem/EcsSystem.h"

class WindowManager;

namespace ascen
{

	using ::WindowManager;

	class Engine
	{
	public:
		Engine(::WindowManager& windowManager);

		VertexRegistry& vertex();
		ResourceRegistry& resource();
		DescriptorRegistry& descriptor();
		PipelineRegistry& pipeline();
		FramePassRegistry& frame();
		RenderTargetRegistry& render();

		EcsSystem& ecs();

		void reload();

		void drawFrame();

		void cleanup();

		uint32_t getScreenWidth() const;
		uint32_t getScreenHeight() const;
		Format getDepthFormat() const;
		uint32_t getFrameIndex() const;
		const std::vector<VkImage>& getPresentImages() const;
		Format getImageFormat() const;

	private:
		::WindowManager& mWindowManager;

		VulkanContext mVulkanContext;
		RenderContext mRenderContext;

		VertexRegistry mVertexRegistry;
		ResourceRegistry mResourceRegistry;
		DescriptorRegistry mDescriptorRegistry;
		PipelineRegistry mPipelineRegistry;
		FramePassRegistry mFramePassRegistry;
		RenderTargetRegistry mRenderTargetRegistry;

		Renderer mRenderer;

		EcsSystem mEcs;
	};

}