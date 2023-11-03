/*! \file ae_simple_render_system.hpp
    \brief The script defining the system that renders 3D objects in the game.
    The simple render system is defined.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "ae_graphics_constants.hpp"

#include "game_components.hpp"
#include "pre_allocated_stack.hpp"


#include <map>


namespace ae {

    /// A child system of the RendererSystem which renders the entity models.
    class AeModel3DBufferSystem : public ae_ecs::AeSystem<AeModel3DBufferSystem> {

    public:
        /// Constructor of the SimpleRenderSystem
        /// \param t_game_components The game components available that this system may require.
        AeModel3DBufferSystem(ae_ecs::AeECS& t_ecs,
                              GameComponents& t_game_components);

        /// Destructor of the SimpleRenderSystem
        ~AeModel3DBufferSystem();

        /// Setup the SimpleRenderSystem, this is handled by the RendererSystem.
        void setupSystem(uint64_t t_frameIndex);

        /// DO NOT CALL! This is not used by this system.
        void setupSystem() override {
            throw std::runtime_error("This is the incorrect execute function for the PointLightRenderSystem and should "
                                     "not have been called. The PointLightRenderSystem execution should be being handled by "
                                     "it's parent, RenderSystem.");
        };

        /// Execute the SimpleRenderSystem, this is handled by the RendererSystem.
        void executeSystem(uint64_t t_frameIndex, std::vector<ecs_id>  t_materialComponentIds);

        /// DO NOT CALL! This is not used by this system.
        void executeSystem() override {
            throw std::runtime_error("This is the incorrect execute function for the PointLightRenderSystem and should "
                                     "not have been called. The PointLightRenderSystem execution should be being handled by "
                                     "it's parent, RenderSystem.");
        };

        /// Clean up the SimpleRenderSystem, this is handled by the RendererSystem.
        void cleanupSystem() override;

        std::vector<Entity3DSSBOData>* getFrameObject3DBufferDataRef(int t_frameIndex){return &m_object3DBufferData[t_frameIndex];};

        std::map<ecs_id, uint64_t>* getObject3DBufferEntityMap(int t_frameIndex){return &m_object3DBufferDataEntityTracking[t_frameIndex];};

    private:

        // Components this system utilizes.
        /// The WorldPositionComponent this systems accesses to know where an entity is to render it.
        WorldPositionComponent& m_worldPositionComponent;
        /// The ModelComponent this system accesses to render the entity in the game world.
        ModelComponent& m_modelComponent;

        /// Stores all the entity specific data for 3D models required for rendering a specific frame.
        std::vector<std::vector<Entity3DSSBOData>> m_object3DBufferData = std::vector<std::vector<Entity3DSSBOData>>(MAX_FRAMES_IN_FLIGHT,std::vector<Entity3DSSBOData>(MAX_OBJECTS, Entity3DSSBOData()));

        /// Maps entities to their model matrix/texture data in the 3D SSBO.
        std::vector<std::map<ecs_id, uint64_t>> m_object3DBufferDataEntityTracking  = std::vector<std::map<ecs_id, uint64_t>>(MAX_FRAMES_IN_FLIGHT,std::map<ecs_id, uint64_t>{});

        /// A stack to track the available data positions in the SSBO.
        PreAllocatedStack<uint64_t,MAX_OBJECTS> m_object3DBufferDataPositionStack[MAX_FRAMES_IN_FLIGHT];

        // Prerequisite systems for the SimpleRenderSystem.
        // This requires any world position updating system to run before this system runs.

        /// Calculates the model, and normal, matrix data.
        /// \param t_translation The translation data for the entity, this normally corresponds to world position but
        /// could be the world position plus an additional offset.
        /// \param t_rotation The rotation of the entity, typically the direction the entity is facing.
        /// \param t_scale The scaling for the entity's model.
        static Entity3DSSBOData calculateModelMatrixData(glm::vec3 t_translation, glm::vec3 t_rotation, glm::vec3 t_scale);
    };
}


