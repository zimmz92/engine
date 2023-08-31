#include "ae_descriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace ae {

    //==================================================================================================================
    // Descriptor Set Layout Builder
    //==================================================================================================================

    // Set the descriptor layout for a binding.
    AeDescriptorSetLayout::Builder& AeDescriptorSetLayout::Builder::addBinding(
        uint32_t t_binding,
        VkDescriptorType t_descriptorType,
        VkShaderStageFlags t_stageFlags,
        uint32_t t_count) {

        // Check to make sure a binding at the specified index has not already been added.
        assert(m_bindings.count(t_binding) == 0 && "Binding already in use");

        // Specify the descriptors' layout.
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = t_binding;
        layoutBinding.descriptorType = t_descriptorType;
        layoutBinding.descriptorCount = t_count;
        layoutBinding.stageFlags = t_stageFlags;

        // Set the binding to the descriptors' layout.
        m_bindings[t_binding] = layoutBinding;

        // Return a pointer to the builder object to allow chaining commands.
        return *this;
    }



    // Create the descriptor set layout based on the m_bindings member variable.
    std::unique_ptr<AeDescriptorSetLayout> AeDescriptorSetLayout::Builder::build() const {
        return std::make_unique<AeDescriptorSetLayout>(m_aeDevice, m_bindings);
    }


    //==================================================================================================================
    // Descriptor Set Layout
    //==================================================================================================================

    // Constructor for the DescriptorSetlayout
    AeDescriptorSetLayout::AeDescriptorSetLayout(
        AeDevice& t_aeDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> t_bindings)
        : m_aeDevice{ t_aeDevice }, m_bindings{ t_bindings } {

        // Create a vector of the set layout bindings.
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : t_bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        // Configure an info struct for the descriptor set layout.
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        // Attempt to create the descriptor set layout.
        if (vkCreateDescriptorSetLayout(
            t_aeDevice.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &m_descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }



    // Destructor for the descriptor set layout.
    AeDescriptorSetLayout::~AeDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(m_aeDevice.device(), m_descriptorSetLayout, nullptr);
    }

    //==================================================================================================================
    // Descriptor Pool Builder
    //==================================================================================================================

    // Specify the required pool size for the specified descriptor type.
    AeDescriptorPool::Builder& AeDescriptorPool::Builder::addPoolSize(
        VkDescriptorType t_descriptorType, uint32_t t_count) {

        // Use implicit construction to add a descriptor pool size struct instance to the pool sizes vector.
        m_poolSizes.push_back({ t_descriptorType, t_count });
        return *this;
    }



    // Set the pool flags to dictate the pool functionality.
    AeDescriptorPool::Builder& AeDescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags t_flags) {

        m_poolFlags = t_flags;
        return *this;
    }



    // Set the maximum number of set that can be allocated from the pool.
    AeDescriptorPool::Builder& AeDescriptorPool::Builder::setMaxSets(uint32_t t_count) {

        m_maxSets = t_count;
        return *this;
    }



    // Build the descriptor pool based on the member variables of the descriptor pool builder.
    std::unique_ptr<AeDescriptorPool> AeDescriptorPool::Builder::build() const {
        return std::make_unique<AeDescriptorPool>(m_aeDevice, m_maxSets, m_poolFlags, m_poolSizes);
    }


    //==================================================================================================================
    // Descriptor Pool
    //==================================================================================================================

    // Create the descriptor pool.
    AeDescriptorPool::AeDescriptorPool(
        AeDevice &t_aeDevice,
        uint32_t t_maxSets,
        VkDescriptorPoolCreateFlags t_poolFlags,
        const std::vector<VkDescriptorPoolSize>& t_poolSizes)
        : m_aeDevice{ t_aeDevice } {

        // Configure a creation information structure for the descriptor pool.
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(t_poolSizes.size());
        descriptorPoolInfo.pPoolSizes = t_poolSizes.data();
        descriptorPoolInfo.maxSets = t_maxSets;
        descriptorPoolInfo.flags = t_poolFlags;

        // Attempt to create the descriptor pool.
        if (vkCreateDescriptorPool(t_aeDevice.device(), &descriptorPoolInfo, nullptr, &m_descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }



    // Clean up the descriptor pool.
    AeDescriptorPool::~AeDescriptorPool() {
        vkDestroyDescriptorPool(m_aeDevice.device(), m_descriptorPool, nullptr);
    }



    // Get a descriptor set from the pool.
    bool AeDescriptorPool::allocateDescriptorSet(
        const VkDescriptorSetLayout t_descriptorSetLayout, VkDescriptorSet& t_descriptor) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.pSetLayouts = &t_descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // TODO create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        // See https://vkguide.dev/docs/extra-chapter/abstracting_descriptors/

        // Attempt to allocate a descriptor set from the descriptor pool.
        if (vkAllocateDescriptorSets(m_aeDevice.device(), &allocInfo, &t_descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }



    // Return a descriptor set to the pool.
    void AeDescriptorPool::freeDescriptorSets(std::vector<VkDescriptorSet>& t_descriptors) const {

        vkFreeDescriptorSets(
            m_aeDevice.device(),
            m_descriptorPool,
            static_cast<uint32_t>(t_descriptors.size()),
            t_descriptors.data());
    }



    // Return all the descriptor sets to the pool.
    void AeDescriptorPool::resetPool() {

        vkResetDescriptorPool(m_aeDevice.device(), m_descriptorPool, 0);
    }

    //==================================================================================================================
    // Descriptor Writer
    //==================================================================================================================

    // Creates the description writer object.
    AeDescriptorWriter::AeDescriptorWriter(AeDescriptorSetLayout& t_setLayout, AeDescriptorPool& t_pool)
        : m_setLayout{ t_setLayout }, m_pool{ t_pool } {};



    // Write data to a buffer type descriptor.
    AeDescriptorWriter& AeDescriptorWriter::writeBuffer(
        uint32_t t_binding, VkDescriptorBufferInfo* t_bufferInfo) {

        // Ensure the binding number is defined in the set layout.
        assert(m_setLayout.m_bindings.count(t_binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = m_setLayout.m_bindings[t_binding];

        // Ensure that the descriptor count in the binding description is just one since this function only deals with
        // writing to a single description.
        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        // Prepare the descriptor write information.
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = t_binding;
        write.pBufferInfo = t_bufferInfo;
        write.descriptorCount = 1;

        // Add the descriptor write information and data to the staging descriptor set.
        m_writes.push_back(write);
        return *this;
    }



    // Write data to an image type descriptor.
    AeDescriptorWriter& AeDescriptorWriter::writeImage(
        uint32_t t_binding, VkDescriptorImageInfo* t_imageInfo) {

        // Ensure the binding number is defined in the set layout.
        assert(m_setLayout.m_bindings.count(t_binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = m_setLayout.m_bindings[t_binding];

        // Ensure that the descriptor count in the binding description is just one since this function only deals with
        // writing to a single description.
        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        // Prepare the descriptor write information.
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = t_binding;
        write.pImageInfo = t_imageInfo;
        write.descriptorCount = 1;

        // Add the descriptor write information and data to the staging descriptor set.
        m_writes.push_back(write);
        return *this;
    }



    // Get a new descriptor set from the pool then write the data to it.
    bool AeDescriptorWriter::build(VkDescriptorSet& t_set) {

        // Allocate a descriptor set from the descriptor set pool.
        bool success = m_pool.allocateDescriptorSet(m_setLayout.getDescriptorSetLayout(), t_set);
        if (!success) {
            return false;
        }

        // Write the descriptor set data to the descriptor set.
        overwrite(t_set);
        return true;
    }



    // Write the data in m_writes to the specified descriptor set.
    void AeDescriptorWriter::overwrite(VkDescriptorSet& t_set) {

        // Set the destination of the data in the staging descriptor set to the allocated descriptor set specified in
        // t_set.
        for (auto& write : m_writes) {
            write.dstSet = t_set;
        }

        // Push the descriptor set data to the GPU.
        vkUpdateDescriptorSets(m_pool.m_aeDevice.device(), m_writes.size(), m_writes.data(), 0, nullptr);
    }

}  // namespace ae
