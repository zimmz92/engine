/// \file ae_graphics_constants.hpp
/// Defines the constants for the graphics engine.
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <memory>
#include "ae_ecs_constants.hpp"
#include <unordered_set>
#include <vulkan/vulkan_core.h>

namespace ae {

    using material_id = std::uint32_t;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    // If MAX_TEXTURES is changed here it also needs to be changed in the shaders!
    // TODO Make the shaders automatically update their values if this value is updated.
    static constexpr int MAX_TEXTURES = 8;
    static constexpr int MAX_TEXTURES_PER_MATERIAL = 10;
    static constexpr int MAX_MATERIALS = 20;
    static constexpr int MAX_OBJECTS = 20;

    static constexpr uint32_t DRAW_INDEXED_INDIRECT_STRING = sizeof(VkDrawIndexedIndirectCommand);

    /// A structure for the 3D SSBO Entity Data.
//    struct Entity3DSSBOData{
//
//        // TODO: Make angles Quaternion
//
//        /// Matrix corresponds to WorldPosition * Ry * Rx * Rz * Scale
//        /// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
//        /// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
//        alignas(16) glm::mat4 modelMatrix{ 1.0f };
//
//        /// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
//        /// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
//        alignas(16) glm::mat4 normalMatrix{ 1.0f };
//
//        /// The index of the objects texture;
//        alignas(1024) uint32_t textureIndex[MAX_MATERIALS][MAX_TEXTURES_PER_MATERIAL]{};
//    };

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
        alignas(4) uint32_t textureIndex = MAX_TEXTURES + 1;
    };

    /// A structure for tracking relevant image buffer information.
    struct ImageBufferInfo{
        ImageBufferInfo(uint64_t t_imageBufferIndex, ecs_id t_entityID, material_id t_materialId){
            m_imageBufferIndex = t_imageBufferIndex;
            m_entityMaterialMap.insert(std::pair<ecs_id,std::unordered_set<material_id>>(t_entityID,{t_materialId}));
        };

        /// Index for a specific image in the image buffer.
        uint64_t m_imageBufferIndex;

        /// For an image records the entities that use the image and which of the entities materials references the
        /// image.
        std::map<ecs_id,std::unordered_set<material_id>> m_entityMaterialMap{};
    };
} // namespace ae