/// \file ae_pool_allocator.cpp
/// The AePoolAllocator class is implemented.
#include "ae_pool_allocator.hpp"

// dependencies

// libraries

// std
#include <cassert>

namespace ae_memory {

    AePoolAllocator::AePoolAllocator(std::size_t const t_allocatedMemorySize,
                                     void *const t_allocatedMemoryPtr,
                                     std::size_t const t_chunkSize,
                                     std::size_t const t_byteAlignment) noexcept:
            m_chunkSize{t_chunkSize},
            m_byteAlignment{t_byteAlignment},
            AeAllocatorBase(t_allocatedMemorySize, t_allocatedMemoryPtr) {

        // Make sure there will be enough size in each block to point to the next free block once freed.
        assert(m_chunkSize >= sizeof(void*) && "Cannot fit the pointer to the next free chunk in a chunk of this size!");

        // Keep track of where the beginning of the memory allocated to this allocator starts.
        m_firstFreeChunkPtr = t_allocatedMemoryPtr;

        // See if the input chunk size works properly with the desired alignment. If not make the chunk size larger to
        // properly align the start of each block with the desired alignment.
        std::size_t chunkAlignment = m_chunkSize % m_byteAlignment;

        if(chunkAlignment != 0){
            m_alignedChunkSize = m_chunkSize + (m_byteAlignment - chunkAlignment);
        } else{
            m_alignedChunkSize = m_chunkSize;
        }

        // Ensure the starting pointer works with the desired alignment. If not, offset it.
        std::size_t requiredAlignment = getAlignmentOffset(m_allocatedMemoryPtr, t_byteAlignment);
        m_firstFreeChunkPtr = addToPointer(requiredAlignment,m_allocatedMemoryPtr);

        // Calculate the number of chunks that can fit into the allocated memory with the required alignment.
        std::size_t maxChunks = (m_allocatedMemorySize-requiredAlignment)/m_alignedChunkSize;

        // Initialize each chunk with a pointer to the next free chunk.
        void** initialPointer = static_cast<void **>(m_firstFreeChunkPtr);
        for(int i=0;i<maxChunks;i++){
            *initialPointer = addToPointer(m_alignedChunkSize,initialPointer);
            initialPointer = (void**)addToPointer(m_alignedChunkSize,initialPointer);
        };
    };


    AePoolAllocator::~AePoolAllocator() noexcept {
        m_firstFreeChunkPtr = nullptr;
    };


    void *AePoolAllocator::allocate(std::size_t const t_allocationSize, std::size_t const t_byteAlignment) {
        assert(t_allocationSize == m_chunkSize && t_byteAlignment == m_byteAlignment);

        // Calculate the total memory that will be used, in bytes, to allocate the desired amount of memory and
        // align the memory.
        std::size_t alignmentOffset = getAlignmentOffset(m_firstFreeChunkPtr, t_byteAlignment);
        std::size_t totalAllocation = alignmentOffset + t_allocationSize;

        // Ensure that this allocation will not exceed the total available memory this stack has available.
        if (totalAllocation + m_memoryInUse > m_allocatedMemorySize) {
            throw std::bad_alloc();
        }

        // Get the aligned address.
        void *alignedAddress = addToPointer(alignmentOffset, m_firstFreeChunkPtr);

        // Increment the top of stack pointer by the total allocation.
        m_firstFreeChunkPtr = addToPointer(totalAllocation, m_firstFreeChunkPtr);

        // Track the additional memory used.
        m_memoryInUse += totalAllocation;

        // Return the aligned address for the allocation.
        return alignedAddress;
    };


    void AePoolAllocator::deallocate([[maybe_unused]] void *const t_allocatedMemoryPtr) noexcept {
        // Traditional deallocation does not work with the stack allocator.
    };
} //namespace ae_memory