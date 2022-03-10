#pragma once

#include "ae_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace ae {

    class AeDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(AeDevice &t_aeDevice) : m_aeDevice{ t_aeDevice } {}

            Builder& addBinding(
                uint32_t t_binding,
                VkDescriptorType t_descriptorType,
                VkShaderStageFlags t_stageFlags,
                uint32_t t_count = 1);
            std::unique_ptr<AeDescriptorSetLayout> build() const;

        private:
            AeDevice& m_aeDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};
        };

        AeDescriptorSetLayout(AeDevice &t_aeDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> t_bindings);
        ~AeDescriptorSetLayout();


        // Do not allow this class to be copied (2 lines below)
        AeDescriptorSetLayout(const AeDescriptorSetLayout&) = delete;
        AeDescriptorSetLayout& operator=(const AeDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }

    private:
        AeDevice &m_aeDevice;
        VkDescriptorSetLayout m_descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

        friend class AeDescriptorWriter;
    };

    class AeDescriptorPool {
    public:
        class Builder {
        public:
            Builder(AeDevice &t_aeDevice) : m_aeDevice{ t_aeDevice } {}

            Builder& addPoolSize(VkDescriptorType t_descriptorType, uint32_t t_count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags t_flags);
            Builder& setMaxSets(uint32_t t_count);
            std::unique_ptr<AeDescriptorPool> build() const;

        private:
            AeDevice &m_aeDevice;
            std::vector<VkDescriptorPoolSize> m_poolSizes{};
            uint32_t m_maxSets = 1000;
            VkDescriptorPoolCreateFlags m_poolFlags = 0;
        };

        AeDescriptorPool(
            AeDevice &t_aeDevice,
            uint32_t t_maxSets,
            VkDescriptorPoolCreateFlags t_poolFlags,
            const std::vector<VkDescriptorPoolSize>& t_poolSizes);
        ~AeDescriptorPool();

        // Do not allow this class to be copied (2 lines below)
        AeDescriptorPool(const AeDescriptorPool&) = delete;
        AeDescriptorPool& operator=(const AeDescriptorPool&) = delete;

        bool allocateDescriptor(const VkDescriptorSetLayout t_descriptorSetLayout, VkDescriptorSet &t_descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet> &t_descriptors) const;

        void resetPool();

    private:
        AeDevice &m_aeDevice;
        VkDescriptorPool m_descriptorPool;

        friend class AeDescriptorWriter;
    };

    class AeDescriptorWriter {
    public:
        AeDescriptorWriter(AeDescriptorSetLayout &t_setLayout, AeDescriptorPool &t_pool);

        AeDescriptorWriter& writeBuffer(uint32_t t_binding, VkDescriptorBufferInfo *t_bufferInfo);
        AeDescriptorWriter& writeImage(uint32_t t_binding, VkDescriptorImageInfo *t_imageInfo);

        bool build(VkDescriptorSet &t_set);
        void overwrite(VkDescriptorSet &t_set);

    private:
        AeDescriptorSetLayout &m_setLayout;
        AeDescriptorPool &m_pool;
        std::vector<VkWriteDescriptorSet> m_writes;
    };

}  // namespace ae