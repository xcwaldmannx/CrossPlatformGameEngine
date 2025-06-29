#include "Buffer.h"

#include <stdexcept>

using namespace vulkan;

const VkBuffer& Buffer::getBuffer() const {
    return mBuffer;
}

const size_t Buffer::size() const {
    return mSize;
}
