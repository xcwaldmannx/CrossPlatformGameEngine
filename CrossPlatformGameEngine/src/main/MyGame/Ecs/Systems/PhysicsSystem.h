#pragma once

#include "../../../EcsSystem/SystemManager/System/System_I.h"

class PhysicsSystem : public System_I
{
public:
  PhysicsSystem();

  void update(float delta) override;

};