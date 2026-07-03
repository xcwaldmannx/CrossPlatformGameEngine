#pragma once

#include "Registry_I.h"

#include <cstdint>
#include <unordered_map>

namespace ascen
{

    template<typename T>
    class Registry : public Registry_I<T>
    {
    public:
        uint64_t registerResource(const T& data) override
        {
            static uint64_t id = 0;
            mResources.emplace(id, data);
            return id;
        }

        void reconstruct() override = 0;

    private:
        std::unordered_map<uint64_t, T> mResources;
    };

}
