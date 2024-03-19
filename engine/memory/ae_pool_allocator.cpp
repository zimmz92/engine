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


        // Calculate the top of the memory range that was allocated. This gets used to ensure this allocator does not
        // attempt to manage memory that does not belong to it.
        m_allocatedMemoryTopPtr = addToPointer(m_allocatedMemorySize,m_allocatedMemoryPtr);

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
            if(i==maxChunks-1){
                // If this is the last chunk of memory available there is no "next available" so set it to nullptr.
                *initialPointer = nullptr;
            } else{
                // Calculate, and store, the pointer to the next available chunk of memory in the current chunk of memory.
                *initialPointer = addToPointer(m_alignedChunkSize,initialPointer);

                // Move to the next chunk of memory.
                initialPointer = (void**)addToPointer(m_alignedChunkSize,initialPointer);
            }

        };
    };


    AePoolAllocator::~AePoolAllocator() noexcept {
        assert(m_memoryInUse==0 && "Huston we have a leak... in a pool... allocator!");

        m_firstFreeChunkPtr = nullptr;
        m_allocatedMemoryTopPtr = nullptr;
    };


    void *AePoolAllocator::allocate(std::size_t const t_allocationSize, std::size_t const t_byteAlignment) {

        // Ensure the right allocator is being used for the allocation.
        assert(t_allocationSize == m_chunkSize && t_byteAlignment == m_byteAlignment);

        // If the stored pointer is a nullptr then there is not more memory to give out.
        if(m_firstFreeChunkPtr == nullptr || m_memoryInUse >= m_allocatedMemorySize){
            throw std::bad_alloc();
        }

        // Get the pointer value to return to the first free chunk of memory.
        m_chunkAllocationPtr = m_firstFreeChunkPtr;

        // Set the first free chunk pointer to the address of the next free chunk of memory which is stored in the
        // initial portion of the current first free chunk of memory.
        m_firstFreeChunkPtr = *(void**)m_firstFreeChunkPtr;

        // Increment the memory in use for the allocated memory.
        m_memoryInUse += m_alignedChunkSize;

        // Return for the allocation.
        return m_chunkAllocationPtr;
    };


    void AePoolAllocator::deallocate(void* const t_allocatedMemoryPtr) noexcept {

        // Ensure the address being deallocated is actually controlled by the allocator.
        assert(t_allocatedMemoryPtr >= m_allocatedMemoryPtr &&
               t_allocatedMemoryPtr < m_allocatedMemoryTopPtr &&
               "Memory being deallocated not in range of memory this allocator controls!");

        assert(m_memoryInUse > 0 && "Cannot deallocate more memory, currently 0 memory is allocated.");

        // Set the initial portion of the memory being deallocated to be a pointer to what is currently the next free
        // memory location. AKA store the pointer to what is currently the first free chunk of memory in the chunk of
        // memory that is being deallocated.
        *(void**)t_allocatedMemoryPtr = m_firstFreeChunkPtr;

        // Set the next available free chunk to the one that was just returned.
        m_firstFreeChunkPtr = t_allocatedMemoryPtr;

        // Decrement the memory in use for the memory being deallocated.
        m_memoryInUse -= m_alignedChunkSize;
    };
} //namespace ae_memory