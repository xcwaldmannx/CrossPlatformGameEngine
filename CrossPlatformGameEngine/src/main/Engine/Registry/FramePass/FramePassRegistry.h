#pragma once

#include "../../FrameGraph/FramePass/FramePass.h"
#include "../../FrameGraph/FramePass/GpuFramePass/GraphicsGpuFramePass/GraphicsGpuFramePass.h"

#include "../Resource/ResourceRegistry.h"
#include "../Descriptor/DescriptorRegistry.h"
#include "../Pipeline/PipelineRegistry.h"

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace ascen
{

	struct FramePassEntry
	{
		std::string mName;
		std::vector<std::string> mDescriptorSets;
		std::string mPipeline;

		std::vector<GpuResource> mResources;
	};

	struct GpuFramePassEntry
	{

	};

	struct GraphicsFramePassEntry : public FramePassEntry
	{
		GraphicsMode mFramePassMode = GraphicsMode::MESH;
	};

	struct ComputeFramePassEntry : public FramePassEntry
	{
		std::array<uint32_t, 3> mGroups{ 1, 1, 1 };
	};

	class FramePassRegistry : public Registry_I
	{
	public:
		void registerGraphics(GraphicsFramePassEntry entry);
		void registerCompute(ComputeFramePassEntry entry);

		void reconstruct() override;

		void cleanup();

	private:
		bool isRegistered(const std::string& name) const;
		bool exists(const std::string& name) const;

	private:
		std::vector<std::string> mRegisteredNames;

		std::vector<GraphicsFramePassEntry> mGraphicsEntries;
		std::vector<ComputeFramePassEntry> mComputeEntries;

		std::unordered_map<std::string, FramePassPtr> mFramePasses;

		friend class FramePassRegistryBackend;
	};

}
