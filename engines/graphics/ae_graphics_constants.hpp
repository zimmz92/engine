/// \file ae_graphics_constants.hpp
/// Defines the constants for the graphics engine.
#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace ae {

    using material_id = std::int64_t;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    // If MAX_TEXTURES is changed here it also needs to be changed in the shaders!
    // TODO Make the shaders automatically update their values if this value is updated.
    static constexpr int MAX_TEXTURES = 8;
    static constexpr int MAX_TEXTURES_PER_MATERIAL = 10;
    static constexpr int MAX_MATERIALS = 20;
    static constexpr int MAX_OBJECTS = 10000;

    /// A structure for the 3D SSBO Entity Data.
    struct Entity3DSSBOData{

        // TODO: Make angles Quaternion

        /// Matrix corresponds to WorldPosition * Ry * Rx * Rz * Scale
        /// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        /// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        alignas(16) glm::mat4 modelMatrix{ 1.0f };

        /// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        /// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        alignas(16) glm::mat4 normalMatrix{ 1.0f };

        /// The index of the objects texture;
        alignas(4) uint32_t textureIndex[MAX_MATERIALS][MAX_TEXTURES_PER_MATERIAL]= {MAX_TEXTURES + 1};
    };
} // namespace ae