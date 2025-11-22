#pragma once

#include "../WindowManager/WindowManager.h"

#include "VulkanContext.h"
#include "RenderContext.h"

#include "Registry/Vertex/VertexRegistry.h"
#include "Registry/Resource/ResourceRegistry.h"
#include "Registry/Descriptor/DescriptorRegistry.h"
#include "Registry/Pipeline/PipelineRegistry.h"
#include "Registry/FramePass/FramePassRegistry.h"

#include "Renderer.h"

#include "RenderGraph/RenderGraph.h"

#include "Resource/Barrier/Barrier.h"

#include "../EcsSystem/EcsSystem.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Components/ModelComponent.h"
#include "Ecs/Systems/RenderSystem.h"

#include <memory>

namespace ascen
{

	class Engine
	{
	public:
		Engine(WindowManager& windowManager);

		VertexRegistry& vertex();
		ResourceRegistry& resource();
		DescriptorRegistry& descriptor();
		PipelineRegistry& pipeline();
		FramePassRegistry& frame();

		EcsSystem& ecs();

		void cleanup();

	private:
		WindowManager& mWindowManager;

		VulkanContext mVulkanContext;
		RenderContext mRenderContext;

		VertexRegistry mVertexRegistry;
		ResourceRegistry mResourceRegistry;
		DescriptorRegistry mDescriptorRegistry;
		PipelineRegistry mPipelineRegistry;
		FramePassRegistry mFramePassRegistry;

		Renderer mRenderer;

		EcsSystem mEcs;
	};

}