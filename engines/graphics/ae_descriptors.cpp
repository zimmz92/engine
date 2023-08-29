#include "ae_descriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace ae {

    // *************** Descriptor Set Layout Builder *********************

    // 
    AeDescriptorSetLayout::Builder& AeDescriptorSetLayout::Builder::addBinding(
        uint32_t t_binding,
        VkDescriptorType t_descriptorType,
        VkShaderStageFlags t_stageFlags,
        uint32_t t_count) {

        assert(m_bindings.count(t_binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = t_binding;
        layoutBinding.descriptorType = t_descriptorType;
        layoutBinding.descriptorCount = t_count;
        layoutBinding.stageFlags = t_stageFlags;
        m_bindings[t_binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<AeDescriptorSetLayout> AeDescriptorSetLayout::Builder::build() const {
        return std::make_unique<AeDescriptorSetLayout>(m_aeDevice, m_bindings);
    }

    // *************** Descriptor Set Layout *********************

    AeDescriptorSetLayout::AeDescriptorSetLayout(
        AeDevice& t_aeDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> t_bindings)
        : m_aeDevice{ t_aeDevice }, m_bindings{ t_bindings } {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : t_bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            t_aeDevice.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &m_descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    AeDescriptorSetLayout::~AeDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(m_aeDevice.device(), m_descriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    AeDescriptorPool::Builder& AeDescriptorPool::Builder::addPoolSize(
        VkDescriptorType t_descriptorType, uint32_t t_count) {
        m_poolSizes.push_back({ t_descriptorType, t_count });
        return *this;
    }

    AeDescriptorPool::Builder& AeDescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags t_flags) {
        m_poolFlags = t_flags;
        return *this;
    }
    AeDescriptorPool::Builder& AeDescriptorPool::Builder::setMaxSets(uint32_t t_count) {
        m_maxSets = t_count;
        return *this;
    }

    std::unique_ptr<AeDescriptorPool> AeDescriptorPool::Builder::build() const {
        return std::make_unique<AeDescriptorPool>(m_aeDevice, m_maxSets, m_poolFlags, m_poolSizes);
    }

    // *************** Descriptor Pool *********************

    AeDescriptorPool::AeDescriptorPool(
        AeDevice &t_aeDevice,
        uint32_t t_maxSets,
        VkDescriptorPoolCreateFlags t_poolFlags,
        const std::vector<VkDescriptorPoolSize>& t_poolSizes)
        : m_aeDevice{ t_aeDevice } {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(t_poolSizes.size());
        descriptorPoolInfo.pPoolSizes = t_poolSizes.data();
        descriptorPoolInfo.maxSets = t_maxSets;
        descriptorPoolInfo.flags = t_poolFlags;

        if (vkCreateDescriptorPool(t_aeDevice.device(), &descriptorPoolInfo, nullptr, &m_descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    AeDescriptorPool::~AeDescriptorPool() {
        vkDestroyDescriptorPool(m_aeDevice.device(), m_descriptorPool, nullptr);
    }

    bool AeDescriptorPool::allocateDescriptor(
        const VkDescriptorSetLayout t_descriptorSetLayout, VkDescriptorSet& t_descriptor) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.pSetLayouts = &t_descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // TODO create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        // See https://vkguide.dev/docs/extra-chapter/abstracting_descriptors/
        if (vkAllocateDescriptorSets(m_aeDevice.device(), &allocInfo, &t_descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void AeDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& t_descriptors) const {
        vkFreeDescriptorSets(
            m_aeDevice.device(),
            m_descriptorPool,
            static_cast<uint32_t>(t_descriptors.size()),
            t_descriptors.data());
    }

    void AeDescriptorPool::resetPool() {
        vkResetDescriptorPool(m_aeDevice.device(), m_descriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    AeDescriptorWriter::AeDescriptorWriter(AeDescriptorSetLayout& t_setLayout, AeDescriptorPool& t_pool)
        : m_setLayout{ t_setLayout }, m_pool{ t_pool } {}

    AeDescriptorWriter& AeDescriptorWriter::writeBuffer(
        uint32_t t_binding, VkDescriptorBufferInfo* t_bufferInfo) {
        assert(m_setLayout.m_bindings.count(t_binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = m_setLayout.m_bindings[t_binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = t_binding;
        write.pBufferInfo = t_bufferInfo;
        write.descriptorCount = 1;

        m_writes.push_back(write);
        return *this;
    }

    AeDescriptorWriter& AeDescriptorWriter::writeImage(
        uint32_t t_binding, VkDescriptorImageInfo* t_imageInfo) {
        assert(m_setLayout.m_bindings.count(t_binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = m_setLayout.m_bindings[t_binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = t_binding;
        write.pImageInfo = t_imageInfo;
        write.descriptorCount = 1;

        m_writes.push_back(write);
        return *this;
    }

    bool AeDescriptorWriter::build(VkDescriptorSet& t_set) {
        bool success = m_pool.allocateDescriptor(m_setLayout.getDescriptorSetLayout(), t_set);
        if (!success) {
            return false;
        }
        overwrite(t_set);
        return true;
    }

    void AeDescriptorWriter::overwrite(VkDescriptorSet& t_set) {
        for (auto& write : m_writes) {
            write.dstSet = t_set;
        }
        vkUpdateDescriptorSets(m_pool.m_aeDevice.device(), m_writes.size(), m_writes.data(), 0, nullptr);
    }

}  // namespace ae
