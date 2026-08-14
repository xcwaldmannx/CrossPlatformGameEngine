#pragma once

#include "../../Core/Types.h"

#include "../../../Utility/FileIO/FileIO.h"

#include "ComputePipeline_I.h"
#include "../../Descriptor/Layout/DescriptorSetLayout.h"

#include <stdexcept>

#include <memory>

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class ComputePipeline : public ComputePipeline_I
    {
    private:
        ComputePipeline(
            VkDevice device,
            const pipeline::ComputeParams& params,
            const std::string& computeShaderFilepath,
            const std::vector<DescriptorSetLayoutPtr>& descriptorSetLayouts) :
            ComputePipeline_I(params, computeShaderFilepath)
        {
            std::vector<VkDescriptorSetLayout> vkLayouts;
            for (const auto& layout : descriptorSetLayouts)
            {
                vkLayouts.push_back(layout->handle());
            }

            handlePipeline(vkLayouts);

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

            mCreateInfo.stage = computeShaderStageInfo;

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

    public:
        void destroy(VkDevice device) override
        {
            vkDestroyPipeline(device, mHandle, nullptr);
            vkDestroyPipelineLayout(device, mLayout, nullptr);
        }

        void uploadPushConstants(const VkCommandBuffer commandBuffer) override
        {
            for (const auto& [id, pc] : mPushConstants)
            {
                vkCmdPushConstants(commandBuffer, mLayout, pc.mShaderStages, pc.mOffset, pc.mSize, pc.mData.get());
            }
        }

    private:
        void handlePipeline(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts)
        {
            // pipeline layout creation
            mLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            mLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
            mLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
            mLayoutInfo.pushConstantRangeCount = 0;
            mLayoutInfo.pPushConstantRanges = nullptr;

            mPushConstants = mParams.mPushConstants;

            for (const auto& [id, pc] : mPushConstants)
            {
                if (pc.mSize > 0)
                {
                    mPushConstantRanges.emplace_back(pc.mShaderStages, pc.mOffset, pc.mSize);
                }
            }

            mLayoutInfo.pushConstantRangeCount = mPushConstantRanges.size();
            mLayoutInfo.pPushConstantRanges = mPushConstantRanges.data();

            // pipeline creation
            mCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            mCreateInfo.stage = mShaderStage;
        }

    protected:
        std::vector<VkPushConstantRange> mPushConstantRanges;

        VkPipelineShaderStageCreateInfo mShaderStage{};
        VkPipelineLayoutCreateInfo mLayoutInfo{};
        VkComputePipelineCreateInfo mCreateInfo{};

        friend class ComputePipelineFactory;
    };
}