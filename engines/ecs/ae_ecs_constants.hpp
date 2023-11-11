/// \file ae_ecs_constants.hpp
/// \brief The script defining constants used by the ecs.
/// Define the constants used by the ecs.

// These constants must come before the includes to ensure that they are available when compiling the included files below
#define MAX_NUM_COMPONENTS 32
#define MAX_NUM_ENTITIES 16384
#define MAX_NUM_SYSTEMS 32

#pragma once

#include <cstdint>

using ecs_id = std::int64_t;
using ecs_systemInterval = std::int64_t;