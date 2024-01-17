/*! \file player_input_system.hpp
    \brief The script defining the player input system.

    The player input system is defined.

*/
#pragma once

// dependencies
#include "ae_model.hpp"
#include "ae_image.hpp"
#include "ae_device.hpp"
#include "ae_ecs_include.hpp"
#include "game_components.hpp"
#include "game_materials.hpp"
#include "ae_samplers.hpp"
#include "game_object_entity.hpp"
#include "ae_window.hpp"

// libraries

//std
#include <memory>


namespace ae {

    static const int destroyCreate = GLFW_KEY_APOSTROPHE;

    /// A system that retrieves the player's inputs and moves/rotates the currently controlled entity.
    class CreateDestroyTestSystem : public ae_ecs::AeSystem<CreateDestroyTestSystem> {
    public:
        /// Constructor of the PlayerInputSystem
        /// \param t_game_components The game components available that this system may require.
        /// \param t_timingSystem The TimingSystem the PlayerInputSystem will depend on executing first
        /// and require information from.
        /// \param t_window A pointer to the GLFW window that will be polling the player's inputs.
        CreateDestroyTestSystem(GLFWwindow* t_window,
                                AeDevice& t_aeDevice,
                                ae_ecs::AeECS& t_aeECS,
                                GameComponents& t_gameComponents,
                                GameMaterials& t_gameMaterials,
                                AeSamplers& t_samplers);

        /// Destructor of the PlayerInputSystem
        ~CreateDestroyTestSystem();

        /// Setup the PlayerInputSystem, this is handled by the ECS.
        void setupSystem() override;

        /// Execute the PlayerInputSystem, this is handled by the ECS.
        void executeSystem() override;

        /// Clean up the PlayerInputSystem, this is handled by the ECS.
        void cleanupSystem() override;


    private:

        static const int numberOfEntities = 2;

        /// The GLFW
        GLFWwindow* m_window;

        /// Tracks the entity IDs of the entities this system created.
        ecs_id m_testEntityIds[numberOfEntities][numberOfEntities] = {0};

        /// Tracks if this system currently has entities created.
        bool m_entitiesCreated = false;

        /// Tracks if this system currently has entities created.
        static bool m_buttonPressed;

        AeDevice& m_aeDevice;
        ae_ecs::AeECS& m_aeECS;
        GameComponents& m_gameComponents;
        GameMaterials& m_gameMaterials;
        AeSamplers& m_aeSamplers;

        /// The model in which will be used to create the entities.
        std::shared_ptr<AeModel> m_aeModel = AeModel::createModelFromFile(m_aeDevice, "assets/models/leaf_enemy_w_tongue.obj");;

        /// The texture which will be used with the model.
        std::shared_ptr<AeImage> m_aeImage = AeImage::createImageFromFile(m_aeDevice,
                                                                          "assets/models/leaf_enemy_body_w_tongue.png");

        // Components this system utilizes.

        // Prerequisite systems for the PlayerInputSystem.


        /// Create a large number of entities. This is to test how the engine handles a large amount of entity creation.
        void createEntities();

        /// Destroy all the entities that were created by this system.  This is to test how the engine handles a large
        /// amount of entity destruction.
        void destroyEntities();

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    };
}


