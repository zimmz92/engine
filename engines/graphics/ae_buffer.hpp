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

        // Do not allow this class to be copied (2 lines below)
        AeBuffer(const AeBuffer&) = delete;
        AeBuffer& operator=(const AeBuffer&) = delete;

        VkResult map(VkDeviceSize t_size = VK_WHOLE_SIZE, VkDeviceSize t_offset = 0);
        void unmap();

        void writeToBuffer(void *t_data, VkDeviceSize t_size = VK_WHOLE_SIZE, VkDeviceSize t_offset = 0);
        VkResult flush(VkDeviceSize t_size = VK_WHOLE_SIZE, VkDeviceSize t_offset = 0);
        VkDescriptorBufferInfo descriptorInfo(VkDeviceSize t_size = VK_WHOLE_SIZE, VkDeviceSize t_offset = 0);
        VkResult invalidate(VkDeviceSize t_size = VK_WHOLE_SIZE, VkDeviceSize t_offset = 0);

        void writeToIndex(void *t_data, int t_index);
        VkResult flushIndex(int t_index);
        VkDescriptorBufferInfo descriptorInfoForIndex(int t_index);
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
        static VkDeviceSize getAlignment(VkDeviceSize t_instanceSize, VkDeviceSize t_minOffsetAlignment);

        AeDevice &m_aeDevice;
        void* m_mapped = nullptr;
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