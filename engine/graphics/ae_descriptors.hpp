#pragma once

#include "ae_engine_constants.hpp"
#include "ae_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace ae {

    /// Provides a pipeline a blueprint of how many descriptor sets to expect and what descriptor types to expect in each
    /// set.
    class AeDescriptorSetLayout {
    public:

        /// Helper class to build the descriptor set layout(s).
        class Builder {
        public:
            /// Creates a builder object for the specified device.
            /// \param t_aeDevice The device that the descriptor set layout is being created for.
            Builder(AeDevice &t_aeDevice) : m_aeDevice{ t_aeDevice } {}

            /// Appends a binding to the descriptor set layout that is being built.
            /// \param t_binding The binding index/location of the descriptor
            /// \param t_descriptorType The vulkan flag defining the type of descriptor being bound.
            /// \param t_stageFlags The flags that indicate which stage(s) of the pipeline the data from this descriptor
            /// will be available to.
            /// \param t_count The number of descriptors of this type that will be in an array at this binding.
            /// \return A pointer to the builder object the binding was added to.
            Builder& addBinding(
                uint32_t t_binding,
                VkDescriptorType t_descriptorType,
                VkShaderStageFlags t_stageFlags,
                uint32_t t_count = 1);

            /// Creates the descriptor set layout from the bindings in m_bindings.
            /// \return A shared pointer to a descriptor set layout for the bindings added to the builder.
            std::shared_ptr<AeDescriptorSetLayout> build() const;

        private:
            /// The device that the bindings are being made for.
            AeDevice& m_aeDevice;

            /// The bindings for the descriptor set layout to be created.
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};
        };

        /// Creates a descriptor set layout for a device from a list of descriptor set layout bindings.
        /// \param t_aeDevice The device te descriptor set layout is being created for.
        /// \param t_bindings The unordered map of bindings for the descriptors being used.
        AeDescriptorSetLayout(AeDevice &t_aeDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> t_bindings);

        /// Destroy the descriptor set layout.
        ~AeDescriptorSetLayout();


        /// Do not allow this class to be copied (2 lines below)
        AeDescriptorSetLayout(const AeDescriptorSetLayout&) = delete;
        AeDescriptorSetLayout& operator=(const AeDescriptorSetLayout&) = delete;

        /// Get the descriptor set layout for this object.
        /// \return A vulkan descriptor set layout.
        VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }

    private:
        /// A reference to the device this descriptor set layout is compatible with.
        AeDevice &m_aeDevice;

        /// The descriptor set layout this object manages.
        VkDescriptorSetLayout m_descriptorSetLayout;

        /// The bindings that comprise the descriptor set layout.
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

        /// Allow the descriptor writer class to directly interact with the bindings to make writing to the bindings
        /// easier.
        friend class AeDescriptorWriter;
    };

    /// A pre-allocated pool of descriptors that can be used to create descriptor sets. This save on performance since
    /// memory allocation is expensive. Pool of pointers to resources which also store some additional data about the
    /// buffer or image being pointed to.
    class AeDescriptorPool {
    public:

        /// Helper class to make creating a descriptor pool a little easier.
        class Builder {
        public:

            /// Creates the builder helper class for creating descriptor pools for the specified device.
            /// \param t_aeDevice The device the descriptor pool will be created for/on.
            Builder(AeDevice &t_aeDevice) : m_aeDevice{ t_aeDevice } {}

            /// Specify how many descriptors of the specified type will be needed in the pool.
            /// \param t_descriptorType The type of descriptor that will be in the pool.
            /// \param t_count The number of descriptors of the specified type that will be in the pool.
            /// \return A reference to the builder object that the pool size was specified for.
            Builder& addPoolSize(VkDescriptorType t_descriptorType, uint32_t t_count);

            /// Specifies the behavior of the pool object.
            /// \param t_flags Flags that will dictate how the pool will behave.
            /// \return A reference to the builder object that the pool behavior was specified for.
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags t_flags);

            /// The total number of descriptor sets that can be allocated from the descriptor pool.
            /// \param t_count The number of descriptor sets that can be allocated from the descriptor pool.
            /// \return A reference to the builder object that the pool set size was specified for.
            Builder& setMaxSets(uint32_t t_count);

            /// Builds the descriptor pool specified by the member variables of this builder object.
            /// \return A unique pointer to the built descriptor pool.
            std::unique_ptr<AeDescriptorPool> build() const;

        private:

            /// The device that the pool is being created for.
            AeDevice &m_aeDevice;

            /// The size of the pool for each of the specific descriptor types in the vector.
            std::vector<VkDescriptorPoolSize> m_poolSizes{};

            /// The maximum number of allowed descriptor sets that can be allocated from the descriptor pool.
            uint32_t m_maxSets = 1000;

            /// The flags that define he desire behavior of the descriptor pool.
            VkDescriptorPoolCreateFlags m_poolFlags = 0;
        };

        /// Creates the descriptor pool management object.
        /// \param t_aeDevice The device the descriptor pool is being made on.
        /// \param t_maxSets The maximum number of sets that maybe allocated from the descriptor pool being created.
        /// \param t_poolFlags The flags that describe the behavior of the descriptor pool being created.
        /// \param t_poolSizes The size of each of descriptor type that will be in the pool
        AeDescriptorPool(
            AeDevice &t_aeDevice,
            uint32_t t_maxSets,
            VkDescriptorPoolCreateFlags t_poolFlags,
            const std::vector<VkDescriptorPoolSize>& t_poolSizes);

        /// Destroy's the descriptor pool.
        ~AeDescriptorPool();

        /// Do not allow this class to be copied (2 lines below)
        AeDescriptorPool(const AeDescriptorPool&) = delete;
        AeDescriptorPool& operator=(const AeDescriptorPool&) = delete;

        /// Get a descriptor set from the pool.
        bool allocateDescriptorSet(const VkDescriptorSetLayout t_descriptorSetLayout, VkDescriptorSet &t_descriptor) const;

        /// Free the descriptor sets, giving them back to the pool to be reallocated later.
        /// \param t_descriptors The descriptors being given back to the pool.
        void freeDescriptorSets(std::vector<VkDescriptorSet> &t_descriptors) const;

        /// Free's all the descriptor sets giving them back to the pool.
        void resetPool();

    private:
        /// The device the descriptor pool will be created on.
        AeDevice &m_aeDevice;

        /// The descriptor pool.
        VkDescriptorPool m_descriptorPool;

        /// Allow the descriptor writer class to directly interact with the pool to make writing to the pool easier.
        friend class AeDescriptorWriter;
    };

    /// Makes building the descriptor objects easier. Allocating a descriptor set from the pool and writing the
    /// necessary information for each descriptor the set contains.
    class AeDescriptorWriter {
    public:

        /// Creates the descriptor writer class for handling descriptor sets from the pool.
        /// \param t_setLayout The descriptor set layout that the writer will be handling.
        /// \param t_pool The descriptor pool this writer will be handling descriptor sets for.
        AeDescriptorWriter(std::shared_ptr<AeDescriptorSetLayout> t_setLayout, AeDescriptorPool &t_pool);

        /// Writes data to a buffer in the descriptor set.
        /// \param t_binding The binding index of the descriptor set the data will be written into.
        /// \param t_bufferInfo The buffer information that is to be written into the descriptor set.
        /// \return A reference to the DescriptorWriter object being used. This is to chain commands easily.
        AeDescriptorWriter& writeBuffer(uint32_t t_binding, VkDescriptorBufferInfo *t_bufferInfo);

        /// Writes data to an image in the descriptor set.
        /// \param t_binding The binding index of the descriptor set the image will be written into.
        /// \param t_bufferInfo The image information that is to be written into the descriptor set.
        /// \return A reference to the DescriptorWriter object being used. This is to chain commands easily.
        AeDescriptorWriter& writeImage(uint32_t t_binding, VkDescriptorImageInfo *t_imageInfo);

        /// A builder function to allocate a descriptor set from the pool.
        /// \param t_set A descriptor set reference that will be populated by this function.
        /// \return True if a descriptor set is successfully allocated from the pool.
        bool build(VkDescriptorSet &t_set);

        /// Writes over any of the data current in the specified descriptor set with the data in the m_writes member
        /// variable.
        /// \param t_set A descriptor set reference to the descriptor set which will have it's data overwritten.
        void overwrite(VkDescriptorSet &t_set);

        /// Clears the data the writer is storing that is to be written to the descriptor
        AeDescriptorWriter& clearWriteData();

    private:
        /// A reference to the descriptor set layout of the descriptor sets this class handles.
        std::shared_ptr<AeDescriptorSetLayout> m_setLayout{};

        /// A reference to the descriptor pool that this class will fetch descriptor sets from and populate with the
        /// specified data.
        AeDescriptorPool &m_pool;

        /// The data that will be written to the descriptor set when the build function is called.s
        std::vector<VkWriteDescriptorSet> m_writes;
    };

}  // namespace ae