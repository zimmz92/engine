/// \file ##.cpp
/// The ## class is implemented.
#include "ae_resource_manager.hpp"

// dependencies

// libraries

// std


namespace ae {

    ResourceManager::ResourceManager(ae::AeDevice &t_aeDevice):m_aeDevice{t_aeDevice} {
    };

    ResourceManager::~ResourceManager() {

    };

    std::shared_ptr<Ae3DModel> ResourceManager::load3DModel(const std::string &t_filepath) {
        return Ae3DModel::createModelFromFile(m_aeDevice,t_filepath);
    };

} //namespace ae