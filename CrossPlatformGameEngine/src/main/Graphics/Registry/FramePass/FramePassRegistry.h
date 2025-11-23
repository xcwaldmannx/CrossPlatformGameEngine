#pragma once

#include "../../FrameGraph/FramePass.h"

#include "../Resource/ResourceRegistry.h"
#include "../Descriptor/DescriptorRegistry.h"
#include "../Pipeline/PipelineRegistry.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace ascen
{

	enum class FramePassType
	{
		GRAPHICS,
		COMPUTE,
	};

	struct FramePassEntry
	{
		std::string mName;
		std::vector<std::string> mVertexBuffers;
		std::string mIndexBuffer;
		std::vector<std::string> mReadBuffers;
		std::vector<std::string> mWriteBuffers;
		std::vector<std::string> mReadTextures;
		std::vector<std::string> mWriteTextures;
		std::vector<std::string> mDescriptorSets;
		std::string mPipeline;
		FramePassType mPipelineType;
	};

	class FramePassRegistry : public Registry_I
	{
	public:
		FramePassRegistry(
			const ResourceRegistry& resourceRegistry,
			const DescriptorRegistry& descriptorRegistry,
			const PipelineRegistry& pipelineRegistry);

		void registerFramePass(FramePassEntry entry);

		void reconstruct() override;

		void cleanup();

	private:
		bool isRegistered(const std::string& name) const;
		bool exists(const std::string& name) const;

	private:
		const ResourceRegistry& mResourceRegistry;
		const DescriptorRegistry& mDescriptorRegistry;
		const PipelineRegistry& mPipelineRegistry;

		std::vector<std::string> mRegisteredNames;

		std::vector<FramePassEntry> mEntries;

		std::unordered_map<std::string, FramePassPtr> mFramePasses;

		friend class FramePassRegistryBackend;
	};

}
