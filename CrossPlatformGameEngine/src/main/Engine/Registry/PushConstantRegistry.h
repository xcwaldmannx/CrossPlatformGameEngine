#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../Pipeline/PushConstant/PushConstantFactory.h"
#include "../Pipeline/PushConstant/PushConstant.h"

namespace ascen
{

    class PushConstantRegistry : public Registry_I<registry::PushConstantEntry, PushConstantPtr>
    {
    public:
        PushConstantRegistry(const PushConstantFactory& pushConstantFactory) :
            mPushConstantFactory(pushConstantFactory) {}

        void reconstruct(const registry::PushConstantEntry &entry, std::shared_ptr<PushConstant> &resource) override
        {
            resource = mPushConstantFactory.create();
        }

    private:
        const PushConstantFactory& mPushConstantFactory;
    };

}
