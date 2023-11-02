/// \file ae_graphics_constants.hpp
/// Defines the constants for the graphics engine.
#pragma once

#include <cstdint>

namespace ae {

    using material_id = std::int64_t;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    // If MAX_TEXTURES is changed here it also needs to be changed in the shaders!
    // TODO Make the shaders automatically update their values if this value is updated.
    static constexpr int MAX_TEXTURES = 8;
    static constexpr int MAX_TEXTURES_PER_MATERIAL = 10;
    static constexpr int MAX_MATERIALS = 20;
    static constexpr int MAX_OBJECTS = 10000;
} // namespace ae