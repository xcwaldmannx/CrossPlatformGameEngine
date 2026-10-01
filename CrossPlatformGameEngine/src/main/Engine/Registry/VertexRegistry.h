#pragma once

#include "../Core/Types.h"
#include "Registry_I.h"
#include "../Vertex/Vertex.h"

#include <memory>

namespace ascen
{

    class VertexRegistry : public Registry_I<registry::VertexEntry, VertexPtr>
    {
    public:
        void reconstruct(const registry::VertexEntry& entry, VertexPtr& resource) override
        {
            resource = std::make_shared<Vertex>(entry.mBinding, entry.mAttributes);
        }
    };
}
