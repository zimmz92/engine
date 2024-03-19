/*! \file ae_simple_render_system.hpp
    \brief The script defining the system that renders 3D objects in the game.
    The simple render system is defined.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "ae_engine_constants.hpp"

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

        /// DO NOT CALL! This is not used by this system.
        void setupSystem() override {
            throw std::runtime_error("This is the incorrect execute function for the PointLightRenderSystem and should "
                                     "not have been called. The PointLightRenderSystem execution should be being handled by "
                                     "it's parent, RenderSystem.");
        };

        /// Execute the SimpleRenderSystem, this is handled by the RendererSystem.
        void executeSystem(std::vector<ecs_id>&  t_materialComponentIds,
                           std::vector<Entity3DSSBOData>& t_object3DBufferData,
                           std::map<ecs_id, uint32_t>& t_object3DBufferEntityMap,
                           PreAllocatedStack<uint64_t,MAX_OBJECTS>& t_object3DBufferDataIndexStack);

        /// DO NOT CALL! This is not used by this system.
        void executeSystem() override {
            throw std::runtime_error("This is the incorrect execute function for the PointLightRenderSystem and should "
                                     "not have been called. The PointLightRenderSystem execution should be being handled by "
                                     "it's parent, RenderSystem.");
        };

        /// Clean up the SimpleRenderSystem, this is handled by the RendererSystem.
        void cleanupSystem() override;

    private:

        // Components this system utilizes.
        /// The WorldPositionComponent this systems accesses to know where an entity is to render it.
        WorldPositionComponent& m_worldPositionComponent;
        /// The ModelComponent this system accesses to render the entity in the game world.
        ModelComponent& m_modelComponent;

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


