/// \file resource_manager.hpp
/// The ResourceManager class is defined. The ResourceManager class ensures that general use assets such as models,
/// images, and audio are created and destroyed as needed and are organize as required to be compatible with aspects
/// of he engine.
#pragma once

// dependencies
#include "ae_3d_model.hpp"

// libraries

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
        std::shared_ptr<Ae3DModel> load3DModel(const std::string& t_filepath);

    private:

        /// The device the resource manager interacts with.
        AeDevice& m_aeDevice;

        /// Keeps track of the currently loaded models.
        std::unordered_map<std::string, std::shared_ptr<Ae3DModel>> m_loadedModels;

    protected:

    };

} // namespace ae


// Notes:
// There are two ways that currently seem viable to do this. The first is to make this a system itself and have it run
// evey frame, the second is to make it a parent class that creates systems for the various different tasks that it may
// be required to do.

// For models, I plan on changing the way importing a model works. Instead of calling the function in the model
// class to import the model the request will be made to this resource manager. The resource manager will then be able
// to decide when the best time to load that resource is... eventually at least. I think this means that the second
// option is better, the one where this is a parent class that may have sub-systems that are run evey frame.

// A change to the model_3d_buffer_system, or perhaps a new system, must be made to ensure that all entities that may
// collide are within the SSBO, not just the ones that can be rendered.

// Perhaps an additional level of hierarchy is required for management of the GPU resources, perhaps in reality this is
// that manager.