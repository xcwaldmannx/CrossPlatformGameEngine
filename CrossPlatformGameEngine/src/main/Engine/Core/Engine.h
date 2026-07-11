#pragma once

#include "VulkanContext.h"
#include "RenderContext.h"

#include "Values.h"

#include "../Registry2/RegistryManager.h"

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

		template<Derived<registry::Entry> E>
		uint64_t registerResource(const E& entry)
		{
			return mRegistryManager.registerResource<E>(entry);
		}

		template<typename T>
		void setPushConstant(const std::string& name, uint32_t pushConstantId, const T& data)
		{
			mRegistryManager.setPushConstant<T>(name, pushConstantId, data);
		}

		void uploadBuffer(
			const std::string& name,
			const void* items,
			const uint32_t itemCount,
			const uint32_t itemSize,
			const uint32_t offset) const;

		void uploadTexture(const std::string& name, const std::vector<unsigned char>& pixels) const;

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

		RegistryManager mRegistryManager;

		Renderer mRenderer;

		EcsSystem mEcs;
	};

}