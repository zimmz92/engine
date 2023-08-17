/*! \file ae_buffer.cpp
    \brief The script implementing the generic vulkan buffer class.
    The buffer class is implemented. Initially based off VulkanBuffer by Sascha Willems -
    https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
*/
#include "ae_buffer.hpp"

// std
#include <cassert>
#include <cstring>

namespace ae {

    // Get the required alignment for the device based on the instance size.
    VkDeviceSize AeBuffer::getAlignment(VkDeviceSize t_instanceSize, VkDeviceSize t_minOffsetAlignment) {

        if (t_minOffsetAlignment > 0) {

            return (t_instanceSize + t_minOffsetAlignment - 1) & ~(t_minOffsetAlignment - 1);
            // TODO: The above code seems to have some error in it. From what I can tell this just returns the instance
            //  size again. See math below. Figure out where this came from then fix accordingly.
            // t_instanceSize = 10001
            // t_minOffsetAlignment = 00111
            // (t_instanceSize + t_minOffsetAlignment - 1) = 10001 + 00111 - 00001 = 10111
            // ~(t_minOffsetAlignment - 1) = 11001
            // (t_instanceSize + t_minOffsetAlignment - 1) & ~(t_minOffsetAlignment - 1) = 10111 & 11001 = 10001
        }

        return t_instanceSize;
    }



    // Construct the buffer object.
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



    // Destroy the buffer object.
    AeBuffer::~AeBuffer() {

        unmap();
        vkDestroyBuffer(m_aeDevice.device(), m_buffer, nullptr);
        vkFreeMemory(m_aeDevice.device(), m_memory, nullptr);
    }



    // Map memory for the buffer.
    VkResult AeBuffer::map(VkDeviceSize t_size, VkDeviceSize t_offset) {

        assert(m_buffer && m_memory && "Called map on buffer before either the buffer has been created or memory "
                                       "allocated for the buffer!");
        return vkMapMemory(m_aeDevice.device(), m_memory, t_offset, t_size, 0, &m_mapped);
    }



    // Unmap the buffer's memory.
    void AeBuffer::unmap() {

        if (m_mapped) {
            vkUnmapMemory(m_aeDevice.device(), m_memory);
            m_mapped = nullptr;
        }
    }



    // Write data to the mapped buffer memory.
    void AeBuffer::writeToBuffer(void *t_data, VkDeviceSize t_size, VkDeviceSize t_offset) {

        assert(m_mapped && "Cannot copy to a buffer that is unmapped to memory!");

        if (t_size == VK_WHOLE_SIZE) {
            memcpy(m_mapped, t_data, m_bufferSize);
        }
        else {
            char* memOffset = (char*)m_mapped;
            memOffset += t_offset;
            memcpy(memOffset, t_data, t_size);
        }
    }



    // Make the specified range of buffer memory on the host available to the device.
    VkResult AeBuffer::flush(VkDeviceSize t_size, VkDeviceSize t_offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_memory;
        mappedRange.offset = t_offset;
        mappedRange.size = t_size;
        return vkFlushMappedMemoryRanges(m_aeDevice.device(), 1, &mappedRange);
    }



    // Invalidate a memory range of the buffer to make it visible to the host.
    VkResult AeBuffer::invalidate(VkDeviceSize t_size, VkDeviceSize t_offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_memory;
        mappedRange.offset = t_offset;
        mappedRange.size = t_size;
        return vkInvalidateMappedMemoryRanges(m_aeDevice.device(), 1, &mappedRange);
    }



    // Create and return a base set of buffer descriptor information based on the provided inputs.
    VkDescriptorBufferInfo AeBuffer::descriptorInfo(VkDeviceSize t_size, VkDeviceSize t_offset) {
        return VkDescriptorBufferInfo{
                m_buffer,
                t_offset,
                t_size,
        };
    }



    // Writes a single instance of data to the buffer at the desired index.
    void AeBuffer::writeToIndex(void* t_data, int t_index) {
        writeToBuffer(t_data, m_instanceSize, t_index * m_alignmentSize);
    }



    // Flush a single instance of data to make it available to the device.
    VkResult AeBuffer::flushIndex(int t_index) {
        return flush(m_alignmentSize, t_index * m_alignmentSize);
    }



    // Get the buffer descriptor information for the specified buffer range.
    VkDescriptorBufferInfo AeBuffer::descriptorInfoForIndex(int t_index) {
        return descriptorInfo(m_alignmentSize, t_index * m_alignmentSize);
    }



    // Invalidate the memory range at index * alignmentSize of the buffer to make it visible to the host.
    VkResult AeBuffer::invalidateIndex(int t_index) {
        return invalidate(m_alignmentSize, t_index * m_alignmentSize);
    }

}  // namespace lve