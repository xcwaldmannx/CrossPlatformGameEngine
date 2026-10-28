#include "CameraSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/CameraComponent.h"

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

CameraSystem::CameraSystem(ascen::Engine& engine) : mEngine(engine) {}

void CameraSystem::update(const float delta)
{
    for (const auto& entity : mEntities)
    {
        const auto& transform = mEngine.ecs().getComponent<TransformComponent>(entity);
        auto& camera = mEngine.ecs().getComponent<CameraComponent>(entity);

        if (mActiveCamera == ENTITY_MAX)
        {
            if (camera.mIsActive) mActiveCamera = entity;
        }
        else if (mActiveCamera != entity && camera.mIsActive)
        {
            auto& old = mEngine.ecs().getComponent<CameraComponent>(mActiveCamera);
            old.mIsActive = false;
            mActiveCamera = entity;
        }

        if (camera.mIsActive)
        {
            camera.mPosition = transform.mPosition + (transform.mRotation * camera.mOffsetPosition);
            camera.mRotation = transform.mRotation * camera.mOffsetRotation;

            const glm::vec3 camForward = camera.mRotation * glm::vec3(0.0f, 0.0f, -1.0f);
            const glm::vec3 camRight = camera.mRotation * glm::vec3(1.0f, 0.0f, 0.0f);
            const glm::vec3 camUp = camera.mRotation * glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 rot(1.0f);
            rot[0] = glm::vec4(camRight, 0.0f);
            rot[1] = glm::vec4(camUp, 0.0f);
            rot[2] = glm::vec4(-camForward, 0.0f);

            const glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), camera.mPosition) * glm::toMat4(camera.mRotation);

            const auto width = static_cast<float>(mEngine.getScreenWidth());
            const auto height = static_cast<float>(mEngine.getScreenHeight());

            if (width * height > 0)
            {
                glm::mat4 view = glm::inverse(cameraTransform);
                glm::mat4 proj = glm::perspective(glm::radians(90.0f), width / height, 1.0f, 100'000.0f);
                proj[1][1] *= -1;

                const glm::mat4 vp = proj * view;
                mEngine.updatePushConstant<glm::mat4>("PIPELINE_FRUSTUM_CULL",  0, vp);
                mEngine.updatePushConstant<glm::mat4>("PIPELINE_RENDER_ENTITY", 0, vp);
                mEngine.updatePushConstant<glm::mat4>("PIPELINE_RENDER_BOUNDS", 0, vp);
                mEngine.updatePushConstant<glm::mat4>("PIPELINE_MOUSE_PICKING", 0, vp);
            }
        }
    }
}

bool CameraSystem::getActiveCamera(CameraComponent& camera) const
{
    if (mActiveCamera != ENTITY_MAX)
    {
        camera = mSystem->getComponent<CameraComponent>(mActiveCamera);
        return true;
    }

    return false;
}
