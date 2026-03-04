#include "CpuFramePass.h"

using namespace ascen;

CpuFramePass::CpuFramePass(
    const FramePassType type,
    const CpuFramePassContext_I* context) :
    FramePass(type),
    mContext(context) {}
