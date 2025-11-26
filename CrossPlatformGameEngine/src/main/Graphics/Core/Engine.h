#pragma once

#include "VulkanContext.h"
#include "RenderContext.h"

#include "../Registry/Vertex/VertexRegistry.h"
#include "../Registry/Resource/ResourceRegistry.h"
#include "../Registry/Descriptor/DescriptorRegistry.h"
#include "../Registry/Pipeline/PipelineRegistry.h"
#include "../Registry/FramePass/FramePassRegistry.h"

#include "../FrameGraph/FrameGraph.h"
#include "Renderer.h"

#include "../Resource/Barrier/Barrier.h"

#include "../../EcsSystem/EcsSystem.h"
#include "../Ecs/Components/TransformComponent.h"
#include "../Ecs/Components/ModelComponent.h"
#include "../Ecs/Systems/RenderSystem.h"

class WindowManager;

namespace ascen
{

	using ::WindowManager;

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

		void reload();
		void updateModelData(std::unordered_map<uint32_t, ModelData>& modelData);

		void drawFrame();

		void cleanup();

		uint32_t getScreenWidth() const;
		uint32_t getScreenHeight() const;
		uint32_t getFrameIndex() const;

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