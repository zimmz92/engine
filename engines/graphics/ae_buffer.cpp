/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */

#include "ae_buffer.hpp"

 // std
#include <cassert>
#include <cstring>

namespace ae {

    /**
     * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
     *
     * @param instanceSize The size of an instance
     * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
     * minUniformBufferOffsetAlignment)
     *
     * @return VkResult of the buffer mapping call
     */
    VkDeviceSize AeBuffer::getAlignment(VkDeviceSize t_instanceSize, VkDeviceSize t_minOffsetAlignment) {
        if (t_minOffsetAlignment > 0) {
            return (t_instanceSize + t_minOffsetAlignment - 1) & ~(t_minOffsetAlignment - 1);
        }
        return t_instanceSize;
    }

    AeBuffer::AeBuffer(
        AeDevice &t_device,
        VkDeviceSize t_instanceSize,
        uint32_t t_instanceCount,
        VkBufferUsageFlags t_usageFlags,
        VkMemoryPropertyFlags t_memoryPropertyFlags,
        VkDeviceSize t_minOffsetAlignment)
        : m_aeDevice{ t_device },
        m_instanceSize{ t_instanceSize },
        m_instanceCount{ t_instanceCount },
        m_usageFlags{ t_usageFlags },
        m_memoryPropertyFlags{ t_memoryPropertyFlags } {

        // Get the buffer alignment to know how to index the buffer data.
        m_alignmentSize = getAlignment(m_instanceSize, t_minOffsetAlignment);

        // Calculate the total size required for the buffer.
        m_bufferSize = m_alignmentSize * m_instanceCount;

        // Create the buffer.
        m_aeDevice.createBuffer(m_bufferSize, m_usageFlags, m_memoryPropertyFlags, m_buffer, m_memory);
    }

    AeBuffer::~AeBuffer() {
        unmap();
        vkDestroyBuffer(m_aeDevice.device(), m_buffer, nullptr);
        vkFreeMemory(m_aeDevice.device(), m_memory, nullptr);
    }

    /**
     * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
     *
     * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
     * buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the buffer mapping call
     */
    VkResult AeBuffer::map(VkDeviceSize t_size, VkDeviceSize t_offset) {
        assert(m_buffer && m_memory && "Called map on buffer before create");
        return vkMapMemory(m_aeDevice.device(), m_memory, t_offset, t_size, 0, &m_mapped);
    }

    /**
     * Unmap a mapped memory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    void AeBuffer::unmap() {
        if (m_mapped) {
            vkUnmapMemory(m_aeDevice.device(), m_memory);
            m_mapped = nullptr;
        }
    }

    /**
     * Copies the specified data to the mapped buffer. Default value writes whole buffer range
     *
     * @param data Pointer to the data to copy
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
     * range.
     * @param offset (Optional) Byte offset from beginning of mapped region
     *
     */
    void AeBuffer::writeToBuffer(void *t_data, VkDeviceSize t_size, VkDeviceSize t_offset) {
        assert(m_mapped && "Cannot copy to unmapped buffer");

        if (t_size == VK_WHOLE_SIZE) {
            memcpy(m_mapped, t_data, m_bufferSize);
        }
        else {
            char* memOffset = (char*)m_mapped;
            memOffset += t_offset;
            memcpy(memOffset, t_data, t_size);
        }
    }

    /**
     * Flush a memory range of the buffer to make it visible to the device
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
     * complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the flush call
     */
    VkResult AeBuffer::flush(VkDeviceSize t_size, VkDeviceSize t_offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_memory;
        mappedRange.offset = t_offset;
        mappedRange.size = t_size;
        return vkFlushMappedMemoryRanges(m_aeDevice.device(), 1, &mappedRange);
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
     * the complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the invalidate call
     */
    VkResult AeBuffer::invalidate(VkDeviceSize t_size, VkDeviceSize t_offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_memory;
        mappedRange.offset = t_offset;
        mappedRange.size = t_size;
        return vkInvalidateMappedMemoryRanges(m_aeDevice.device(), 1, &mappedRange);
    }

    /**
     * Create a buffer info descriptor
     *
     * @param size (Optional) Size of the memory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkDescriptorBufferInfo of specified offset and range
     */
    VkDescriptorBufferInfo AeBuffer::descriptorInfo(VkDeviceSize t_size, VkDeviceSize t_offset) {
        return VkDescriptorBufferInfo{
            m_buffer,
            t_offset,
            t_size,
        };
    }

    /**
     * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
     *
     * @param data Pointer to the data to copy
     * @param index Used in offset calculation
     *
     */
    void AeBuffer::writeToIndex(void* t_data, int t_index) {
        writeToBuffer(t_data, m_instanceSize, t_index * m_alignmentSize);
    }

    /**
     *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
     *
     * @param index Used in offset calculation
     *
     */
    VkResult AeBuffer::flushIndex(int t_index) { return flush(m_alignmentSize, t_index * m_alignmentSize); }

    /**
     * Create a buffer info descriptor
     *
     * @param index Specifies the region given by index * alignmentSize
     *
     * @return VkDescriptorBufferInfo for instance at index
     */
    VkDescriptorBufferInfo AeBuffer::descriptorInfoForIndex(int t_index) {
        return descriptorInfo(m_alignmentSize, t_index * m_alignmentSize);
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param index Specifies the region to invalidate: index * alignmentSize
     *
     * @return VkResult of the invalidate call
     */
    VkResult AeBuffer::invalidateIndex(int t_index) {
        return invalidate(m_alignmentSize, t_index * m_alignmentSize);
    }

}  // namespace lve