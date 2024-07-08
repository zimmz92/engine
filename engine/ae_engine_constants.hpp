/// \file ae_graphics_constants.hpp
/// Defines the constants, and common data structures, for the graphics engine.
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <memory>
#include "ae_ecs_constants.hpp"
#include <unordered_set>
#include <vulkan/vulkan_core.h>

/// The maximum number of frames allowed to be in the rendering queue at a given time.
/// If MAX_TEXTURES is changed here it also needs to be changed in the shaders!
// TODO Make the shaders automatically update their values if this value is updated.v
static const std::size_t MAX_FRAMES_IN_FLIGHT = 2;

/// The maximum number of textures the engine will support loaded into the texture SSBO.
static const std::size_t MAX_TEXTURES = 8;

/// Defines the maximum allowed materials that are applicable to 3D objects, and the maximum number of textures that any
/// one 3D material may have as part of it's pipeline shaders.
static const std::size_t MAX_3D_MATERIALS = 20;
static const std::size_t MAX_3D_MATERIAL_TEXTURES = 10;

/// Defines the maximum allowed materials that are applicable to 2D objects, and the maximum number of textures that any
/// one 2D material may have as part of it's pipeline shaders.
static const std::size_t MAX_2D_MATERIALS = 20;
static const std::size_t MAX_2D_MATERIAL_TEXTURES = 10;

/// The maximum number of objects that are allowed to be rendered at a given time.
static const std::size_t MAX_OBJECTS = 16384;

/// The maximum number of models that are allowed to be loaded at a given time.
static const std::size_t MAX_MODELS = 1024;

/// The maximum number of allowed gpu particles at a single time
static const std::size_t MAX_PARTICLES = 1024;

namespace ae {

    /// A defined type for the material ID. uint32_t used here to be compatible with shader integer input.
    using material_id = std::uint32_t;

    /// A defined type for SSBO indicies.
    using ssbo_idx = std::uint32_t;

    /// Making the draw indirect command size a constant to reduce unnecessary calls to get the size of the command.
    static constexpr uint32_t DRAW_INDEXED_INDIRECT_COMMAND_SIZE = sizeof(VkDrawIndexedIndirectCommand);

    /// A structure for the 3D SSBO Entity Data.
    struct Entity3DSSBOData{

        // TODO: Make angles Quaternion

        /// Matrix corresponds to WorldPosition * Ry * Rx * Rz * Scale
        /// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        /// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 modelMatrix{ 1.0f };

        /// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        /// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 normalMatrix{ 1.0f };

        /// The index of the 3D objects textures for a given material.
        //alignas(16) uint32_t textureIndex[MAX_3D_MATERIALS][MAX_3D_MATERIAL_TEXTURES]{};
        uint32_t textureIndex[MAX_3D_MATERIALS][MAX_3D_MATERIAL_TEXTURES]{};

        /// The AABB of this 3D object.
        VkAabbPositionsKHR aabb = {0.0f};

        /// The index of the OBB for the 3D object's model.
        uint32_t modelObbIndex = 0;

        /// Spacer to ensure that the SSBO data array alignment requirements are satisfied.
        float spacer1[1] = {0.0f};
    };

    /// A structure to store all the OBB data for the 3D models.
    struct Model3DObbData{
        /// The OBB of a 3D model.
        VkAabbPositionsKHR obb = {0.0f};

        /// Spacer to ensure that the SSBO data array alignment requirements are satisfied.
        float spacer1[2] = {0.0f};
    };



    /// An SSBO data structure for point light properties.
    struct PointLightSSBOData{
        alignas(16) glm::vec4 position{};
        alignas(16) glm::vec4 color{};
        alignas(4) float radius;
    };


    /// An SSBO data structure for 2D object properties.
    struct Entity2DSSBOData {
        /// The scaling and rotation of the 2D object.
        alignas(16) glm::mat2 transform{ 1.0f };

        /// The offset of the 2D object.
        alignas(16) glm::vec2 translation{ 1.0f };

        /// The index of the 2D objects texture for a given material.
        alignas(32) uint32_t textureIndex[MAX_2D_MATERIALS][MAX_2D_MATERIAL_TEXTURES]{};
    };


    /// A image buffer structure that tracks an images position within the image buffer and which entities use the image
    /// with which materials.
    struct ImageBufferInfo{

        /// Creates an image buffer structure to track a image's position within the image buffer and which entities use
        /// the image with which materials.
        /// \param t_imageBufferIndex The index of the image in the image buffer.
        /// \param t_entityID The entity ID that uses the image at the specified index.
        /// \param t_materialId The material ID corresponding to the material the entity is using the image with.
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