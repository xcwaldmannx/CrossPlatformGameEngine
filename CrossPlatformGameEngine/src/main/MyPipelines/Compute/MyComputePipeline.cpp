#include "MyComputePipeline.h"

MyComputePipeline::MyComputePipeline(
    const std::string& computeShaderFilepath,
    VkDescriptorSetLayout descriptorSetLayout) :
    ascen::ComputePipeline(computeShaderFilepath, descriptorSetLayout) {}

void MyComputePipeline::createDescriptorResources()
{

}

void MyComputePipeline::destroyDescriptorResources()
{

}
