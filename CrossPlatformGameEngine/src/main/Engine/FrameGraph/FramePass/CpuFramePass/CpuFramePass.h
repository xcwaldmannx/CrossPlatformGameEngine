#pragma once

#include "../FramePass.h"

#include <span>
#include <string_view>

namespace ascen
{

    struct CpuData
    {
        void* ptr = nullptr;
        size_t bytes = 0;
        size_t count = 0;
        size_t stride = 0;
    };

    struct CpuFramePassContext_I
    {
        virtual ~CpuFramePassContext_I() = default;

        virtual CpuData readBytes(std::string_view name) const = 0;
        virtual CpuData writeBytes(std::string_view name, size_t elemCount, size_t elemStride) = 0;

        template<typename T>
        std::span<const T> read(const std::string_view name) const
        {
            static_assert(std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>);
            auto v = readBytes(name);
            return { static_cast<const T*>(v.ptr), v.count };
        }

        template<typename T>
        std::span<T> write(const std::string_view name, size_t count)
        {
            static_assert(std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>);
            auto v = writeBytes(name, count, sizeof(T));
            return { static_cast<T*>(v.ptr), count };
        }
    };

    struct CpuFramePass : FramePass
    {
        CpuFramePass(
            const FramePassType type,
            const CpuFramePassContext_I* context);

        const CpuFramePassContext_I* mContext;
    };

}