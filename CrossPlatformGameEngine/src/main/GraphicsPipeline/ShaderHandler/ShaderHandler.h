#pragma once

#include "../LogicalDeviceHandler/LogicalDeviceHandler.h"

#include <vector>

#include <Vulkan/vulkan.h>

struct ShaderModule {
	VkShaderModule mShaderModule;
};

struct ShaderStage
{
	VkShaderModule mModule;
	VkPipelineShaderStageCreateInfo mInfo;
};

namespace ascen {

	/*
	FILE: ShaderHandler.h
	NAME: createShader
	RTRN: void
	DESC: Handles Shader Module creation
	*/
	void createShader(
		LogicalDevice& logicalDevice,
		const std::vector<char>& shaderCode,
		ShaderModule* module);

	/*
	FILE: ShaderHandler.h
	NAME: destroyShader
	RTRN: void
	DESC: Handles Shader Module destruction
	*/
	void destroyShader(
		LogicalDevice& logicalDevice,
		ShaderModule& module);

	void createShaderStage(
		VkDevice device,
		const std::vector<char>& byteCode,
		VkShaderStageFlagBits stageFlags,
		ShaderStage* stage);

	void destroyShaderStage(
		VkDevice device,
		ShaderStage& module);

};
