/*! \file ae_buffer.hpp
    \brief The script defining the generic vulkan buffer class.
    The buffer class is defined. Initially based off VulkanBuffer by Sascha Willems -
    https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
*/
#pragma once

#include "ae_device.hpp"

namespace ae {

    class AeBuffer {
    public:
        AeBuffer(
                AeDevice &t_device,
                VkDeviceSize t_instanceSize,
                uint32_t t_instanceCount,
                VkBufferUsageFlags t_usageFlags,
                VkMemoryPropertyFlags t_memoryPropertyFlags,
                VkDeviceSize t_minOffsetAlignment = 1);

        ~AeBuffer();

        /// Do not allow this class to be copied (2 lines below)
        AeBuffer(const AeBuffer &) = delete;
        AeBuffer &operator=(const AeBuffer &) = delete;

        /// Map a memory range for this buffer. If successful, the class pointer to the memory will be populated.
        /// \param t_size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
        /// buffer range.
        /// \param t_offset (Optional) Byte offset from beginning
        /// \return VkResult of the buffer mapping call.
        VkResult map(VkDeviceSize t_size = VK_WHOLE_SIZE, VkDeviceSize t_offset = 0);

        /// Unmap the mapped memory for the buffer.
        /// \note This does not not return a result as vkUnmapMemory can't fail.
        void unmap();

        /// Copies the specified data to the mapped buffer. By default this writes the whole buffer range and will
        /// assume no offset.
        /// \param t_data Pointer to the data to copy.
        /// \param t_size (Optional) Size of the data to copy.
        /// \param t_offset (Optional) Byte offset from beginning of mapped region.
        void writeToBuffer(void *t_data, VkDeviceSize t_size = VK_WHOLE_SIZE, VkDeviceSize t_offset = 0);

        /// Flush a memory range of the buffer to make it visible to the device.
        /// \note Only required for non-coherent memory.
        /// \param t_size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
        /// complete buffer range.
        /// \param t_offset (Optional) Byte offset from beginning of the buffer.
        /// \return VK_SUCCESS if the the flush call was successful.
        VkResult flush(VkDeviceSize t_size = VK_WHOLE_SIZE, VkDeviceSize t_offset = 0);

        /// Create a buffer info descriptor.
        /// \param t_size (Optional) Size of the memory range of the descriptor.
        /// \param t_offset (Optional) Byte offset from beginning.
        /// \return VkDescriptorBufferInfo of specified offset and range.
        VkDescriptorBufferInfo descriptorInfo(VkDeviceSize t_size = VK_WHOLE_SIZE, VkDeviceSize t_offset = 0);

        /// Invalidate a memory range of the buffer to make it visible to the host. Ensures that device writes to the
        /// memory can be seen by the host.
        /// \note Only required for non-coherent memory
        /// \param t_size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
        /// the complete buffer range.
        /// \param t_offset (Optional) Byte offset from beginning
        /// \return VK_SUCCESS if the invalidate call was successful.
        VkResult invalidate(VkDeviceSize t_size = VK_WHOLE_SIZE, VkDeviceSize t_offset = 0);

        /// Copies a single "instanceSize" of data to the mapped buffer at the memory offset of index * alignmentSize.
        /// \param t_data Pointer to the data to copy
        /// \param t_index Used in offset calculation
        void writeToIndex(void *t_data, int t_index);


        /// Flush the memory range at index * alignmentSize of the buffer to make it visible to the device.
        /// \param t_index The location of the memory range to be flushed.
        /// \return VK_SUCCESS if the flush call was successful.
        VkResult flushIndex(int t_index);

        /// Create a buffer info descriptor for the buffer region given by index * alignmentSize.
        /// \param t_index Specifies the region given by index * alignmentSize.
        /// \return VkDescriptorBufferInfo for instance at index.
        VkDescriptorBufferInfo descriptorInfoForIndex(int t_index);

        /// Invalidate the memory range at index * alignmentSize of the buffer to make it visible to the host.
        /// \note Only required for non-coherent memory.
        /// \param t_index Specifies the region to invalidate: index * alignmentSize.
        /// \return VK_SUCCESS of the invalidate call.
        VkResult invalidateIndex(int t_index);

        VkBuffer getBuffer() const { return m_buffer; }

        void *getMappedMemory() const { return m_mapped; }

        uint32_t getInstanceCount() const { return m_instanceCount; }

        VkDeviceSize getInstanceSize() const { return m_instanceSize; }

        VkDeviceSize getAlignmentSize() const { return m_instanceSize; }

        VkBufferUsageFlags getUsageFlags() const { return m_usageFlags; }

        VkMemoryPropertyFlags getMemoryPropertyFlags() const { return m_memoryPropertyFlags; }

        VkDeviceSize getBufferSize() const { return m_bufferSize; }

    private:
        /// Returns the minimum instance size required to be compatible with devices minOffsetAlignment.
        /// \param t_instanceSize The size of an instance
        /// \param t_minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
        /// minUniformBufferOffsetAlignment)
        /// \return VkResult of the buffer mapping call.
        static VkDeviceSize getAlignment(VkDeviceSize t_instanceSize, VkDeviceSize t_minOffsetAlignment);

        AeDevice &m_aeDevice;
        void *m_mapped = nullptr;
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        VkDeviceSize m_bufferSize;
        uint32_t m_instanceCount;
        VkDeviceSize m_instanceSize;
        VkDeviceSize m_alignmentSize;
        VkBufferUsageFlags m_usageFlags;
        VkMemoryPropertyFlags m_memoryPropertyFlags;
    };

}  // namespace lve