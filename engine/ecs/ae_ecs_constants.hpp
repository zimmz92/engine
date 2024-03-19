/// \file ae_ecs_constants.hpp
/// \brief The script defining constants used by the ecs.
/// Define the constants used by the ecs.

#pragma once

#include <cstdint>
#include <limits>

using ecs_id = std::size_t;
using ecs_systemInterval = std::size_t;

static const ecs_id MAX_NUM_COMPONENTS = 32;
static const ecs_id MAX_NUM_ENTITIES = 16000;
//static const ecs_id MAX_NUM_ENTITIES = 65536;
static const ecs_id MAX_NUM_SYSTEMS = 32;