/// \file ##.cpp
/// The ## class is implemented.
#include "ae_resource_manager.hpp"

// dependencies

// libraries

// std
#include <stdexcept>

namespace ae {

    AeResourceManager::AeResourceManager(ae::AeDevice &t_aeDevice): m_aeDevice{t_aeDevice} {
    };

    AeResourceManager::~AeResourceManager() {

    };

    std::shared_ptr<Ae3DModel> AeResourceManager::use3DModel(const std::string &t_filepath) {
        // Check if the model is already loaded.
        auto loadedModelIterator = m_loadedModels.find(t_filepath);

        if(loadedModelIterator==m_loadedModels.end()){
            // If the model is not already loaded create a new model and track its usage.
            // TODO: Using the shared pointer method gives me no control to load or unload modes as desired.
            std::shared_ptr<Ae3DModel> loadedModel = Ae3DModel::createModelFromFile(m_aeDevice,t_filepath,m_object3DBufferDataIndexStack.pop());
            m_loadedModels[t_filepath] = loadedModel;
            loadedModel->incrementNumUsers();
            return loadedModel;
        } else{
            // Otherwise return the location of the loaded model and track the additional reference.
            loadedModelIterator->second->incrementNumUsers();
            return loadedModelIterator->second;
        }
    };

    void AeResourceManager::unuse3DModel(const std::shared_ptr<Ae3DModel>& t_model){
        if(t_model->getNumUsers() >= 1){
            t_model->decrementNumUsers();
            if(t_model->getNumUsers() == 0){
                unloadModel(t_model);
            }
        }
        else{
            throw std::runtime_error("Decrementing number of model users would result in less than 0!!");
        }
    };

    void AeResourceManager::unloadModel(std::shared_ptr<Ae3DModel> t_model){

        bool model_found = false;
        std::string model_path;

        // Traverse the map
        for (auto& it : m_loadedModels) {
            // If mapped value is K,
            // then print the key value
            if (it.second == t_model) {
                model_path = it.first;
            }
        }
        if(model_found){
            m_loadedModels.erase(model_path);
        } else{
            throw std::runtime_error("Can not find model to be removed in map of loaded models!!");
        }
    };
} //namespace ae