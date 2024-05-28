/// \file ##.cpp
/// The ## class is implemented.
#include "ae_resource_manager.hpp"

// dependencies

// libraries

// std
#include <stdexcept>

namespace ae {

    ResourceManager::ResourceManager(ae::AeDevice &t_aeDevice):m_aeDevice{t_aeDevice} {
    };

    ResourceManager::~ResourceManager() {

    };

    std::shared_ptr<Ae3DModel> ResourceManager::use3DModel(const std::string &t_filepath) {
        // Check if the model is already loaded.
        auto loadedModelIterator = m_loadedModels.find(t_filepath);

        if(loadedModelIterator==m_loadedModels.end()){
            // If the model is not already loaded. create a new model and track its usage.
            std::shared_ptr<Ae3DModel> loadedModel = Ae3DModel::createModelFromFile(m_aeDevice,t_filepath);
            m_loadedModels[t_filepath] = loadedModel;
            m_modelReferences[loadedModel] = 1;
            return loadedModel;
        } else{
            // Otherwise return the location of the loaded model and track the additional reference.
            m_modelReferences[loadedModelIterator->second] += 1;
            return loadedModelIterator->second;
        }
    };

    void ResourceManager::unuse3DModel(const std::shared_ptr<Ae3DModel>& t_model){
        if(m_modelReferences[t_model]>=1){
            m_modelReferences[t_model] -= 1;
        }
        else{
            throw std::runtime_error("Too many models have been returned! Returning this model would decrement the "
                                     "count to less than zero!");
        }
    };

} //namespace ae