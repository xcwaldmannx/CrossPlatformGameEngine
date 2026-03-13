#include "FramePass.h"

using namespace ascen;



FramePass::FramePass(const FramePassType type) :
    mType(type) {}

const std::string FramePass::getStringType() const
{
    switch (mType)
    {
        case FramePassType::CPU:
            return "CPU";
        case FramePassType::GRAPHICS:
            return "GRAPHICS";
        case FramePassType::COMPUTE:
            return "COMPUTE";
        case FramePassType::TRANSFER:
            return "TRANSFER";
        case FramePassType::NONE:
        default:
            return "NONE";
    }
}