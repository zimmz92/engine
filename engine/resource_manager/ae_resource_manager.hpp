/// \file resource_manager.hpp
/// The ResourceManager class is defined. The ResourceManager class ensures that general use assets such as models,
/// images, and audio are created and destroyed as needed and are organize as required to be compatible with aspects
/// of he engine.
#pragma once

// dependencies
#include "ae_3d_model.hpp"
#include "ae_de_stack_allocator.hpp"
#include "ae_free_linked_list_allocator.hpp"

// libraries
#include "pre_allocated_stack.hpp"

// std
#include <unordered_map>
#include <string>

namespace ae {

    class ResourceManager {
    public:
        /// Is responsible for handling the creation and destruction of general use assets and keeping them organized.
        /// Examples of these types of assets include models, images, and audio.
        ResourceManager(AeDevice& t_aeDevice);

        ~ResourceManager();

        /// Loads the model (file/path/filename.obj) at the specified location.
        std::shared_ptr<Ae3DModel> use3DModel(const std::string& t_filepath);

        void unuse3DModel(const std::shared_ptr<Ae3DModel>& t_model);


    private:

        /// The device the resource manager interacts with.
        AeDevice& m_aeDevice;

        /// Keeps track of the currently loaded 3D models.
        std::unordered_map<std::string, std::shared_ptr<Ae3DModel>> m_loadedModels;

        //==============================================================================================================
        // Oriented Bounding Box (OBB) Shader Storage Buffer Object (SSBO)
        //==============================================================================================================

        /// A stack to track the available data positions in the SSBO.
        PreAllocatedStack<ssbo_idx,MAX_MODELS> m_object3DBufferDataIndexStack{};

        void unloadModel(std::shared_ptr<Ae3DModel> t_model);

        /// Primary Stack Allocator for the game
        //std::size_t m_deStackAllocationSize = 4000000000;
        //void* m_deStackAllocation = malloc(m_deStackAllocationSize);
        //ae_memory::AeDeStackAllocator m_deStackAllocator{m_deStackAllocationSize,m_deStackAllocation};

        /// Primary Free List Allocator for the game
        //std::size_t m_freeLinkedListAllocationSize = 1000000;
        //void* m_freeLinkedListAllocation = malloc(m_freeLinkedListAllocationSize);
        //ae_memory::AeFreeLinkedListAllocator m_freeLinkedListAllocator{m_freeLinkedListAllocationSize,m_freeLinkedListAllocation};

    protected:

    };

} // namespace ae


// Notes:
// The resource manager will be in charge of managing memory in general, this is both GPU memory and RAM. As a result it
// will be what is called to create an object. This will require a re-factoring of the ECS to request memory from the
// resource manager, however this will allow for easier visualization of memory usage across the engine.
//
// The architecture of the resource manager is that it is a parent class that will contain one or more systems that will
// ensure memory resources are distributed as needed.
//
// For models, I plan on changing the way importing a model works. Instead of calling the function in the model
// class to import the model the request will be made to this resource manager. The resource manager will then be able
// to decide when the best time to load that resource is... eventually at least. I think this means that the second
// option is better, the one where this is a parent class that may have sub-systems that are run evey frame.
//
// A change to the model_3d_buffer_system, or perhaps a new system, must be made to ensure that all entities that may
// collide are within the SSBO, not just the ones that can be rendered.

// Need an index into the OBB SSBO for each model which associates an object with its model OBB.
// Think it might be time for some sort of resource manager... allows for model OBB SSBO to be handled by the resource manager.
//      - Resource manager knows where all models are and which are available/ need to be initialized.
//      - When a model is loaded the OBB is initialized and placed into the OBB SSBO and the index is stored in the model object.
//      - Do the same thing with Images.