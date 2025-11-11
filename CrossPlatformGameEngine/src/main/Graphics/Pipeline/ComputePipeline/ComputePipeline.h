#pragma once

#include "ComputePipeline_I.h"
#include "../../../Utility/FileIO/FileIO.h"

#include <concepts>
#include <stdexcept>

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class ComputePipeline : public ComputePipeline_I
    {
    public:
        ComputePipeline(
            const std::string& computeShaderFilepath,
            VkDescriptorSetLayout descriptorSetLayout) :
            ComputePipeline_I(computeShaderFilepath)
        {
            mDescriptorSetLayouts = { descriptorSetLayout };

            handlePipeline();
        }

        void create(VkDevice device) override
        {
            auto computeShaderCode = FileIO::readFile(mComputeShaderFilepath);

            VkShaderModuleCreateInfo computeShaderModuleInfo{};
            computeShaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            computeShaderModuleInfo.codeSize = computeShaderCode.size();
            computeShaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(computeShaderCode.data());

            VkShaderModule computeShaderModule;
            if (vkCreateShaderModule(device, &computeShaderModuleInfo, nullptr, &computeShaderModule) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create vertex shader stage!");
            }

            VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
            computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            computeShaderStageInfo.module = computeShaderModule;
            computeShaderStageInfo.pName = "main";

            mShaderStage = computeShaderStageInfo;

            // create pipeline layout and pipeline
            if (vkCreatePipelineLayout(
                device, &mLayoutInfo, nullptr, &mLayout) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create compute pipeline layout!");
            }

            mCreateInfo.layout = mLayout;

            if (vkCreateComputePipelines(
                device, nullptr, 1, &mCreateInfo, nullptr, &mHandle) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create compute pipeline!");
            }

            // destroy the shader
            vkDestroyShaderModule(device, computeShaderModule, nullptr);
        }

        void destroy(VkDevice device) override
        {
            vkDestroyPipeline(device, mHandle, nullptr);
            vkDestroyPipelineLayout(device, mLayout, nullptr);
        }

    private:
        void handlePipeline()
        {
            // pipeline layout creation
            mLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            mLayoutInfo.setLayoutCount = static_cast<uint32_t>(mDescriptorSetLayouts.size());
            mLayoutInfo.pSetLayouts = mDescriptorSetLayouts.data();

            // pipeline creation
            mCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            mCreateInfo.stage = mShaderStage;
        }

    protected:
        VkPipelineShaderStageCreateInfo mShaderStage;
        std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
        VkPipelineLayoutCreateInfo mLayoutInfo{};
        VkComputePipelineCreateInfo mCreateInfo{};

    };
}