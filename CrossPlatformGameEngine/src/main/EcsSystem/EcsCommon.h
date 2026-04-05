#pragma once

#include <cstdint>

#include <bitset>

const uint32_t ENTITY_MIN = 0;
const uint32_t ENTITY_MAX = 256'000;

const uint32_t COMPONENT_MIN = 0;
const uint32_t COMPONENT_MAX = 128;

using EntityId = uint32_t;

using Signature = std::bitset<COMPONENT_MAX>;