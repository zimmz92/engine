/// \file ae_free_list_allocator.cpp
/// The AeFreeListAllocator class is implemented.
#include "ae_free_linked_list_allocator.hpp"

// dependencies

// libraries

// std
#include <cassert>

namespace ae_memory {

    AeFreeLinkedListAllocator::AeFreeLinkedListAllocator(std::size_t const t_allocatedMemorySize,
                                                         void *const t_allocatedMemoryPtr) noexcept:
            AeAllocatorBase(t_allocatedMemorySize, t_allocatedMemoryPtr) {

        assert(t_allocatedMemorySize > sizeof(FreeChunkInfo) && "Memory allocated to AeFreeLinkedListAllocator is not"
                                                                " big enough for the FreeChunkInfo struct.");

        // Calculate the top of the range of memory that was allocated. This gets used to ensure this allocator does not
        // attempt to manage memory that does not belong to it.
        m_allocatedMemoryTail = addToPointer(m_allocatedMemorySize, m_allocatedMemoryPtr);

        // Create the initial free chunk which is just the entire memory that has been given to this allocator to
        // manage.
        m_firstFreeChunkPtr = static_cast<FreeChunkInfo *>(t_allocatedMemoryPtr);
        m_firstFreeChunkPtr->m_chunkSize = t_allocatedMemorySize;
        m_firstFreeChunkPtr->m_nextFreeChunk = nullptr;
    };


    AeFreeLinkedListAllocator::~AeFreeLinkedListAllocator() noexcept {
        assert(m_memoryInUse == 0 && "Huston we have a leak... in a free linked list... allocator!");

        m_firstFreeChunkPtr = nullptr;
        m_allocatedMemoryTail = nullptr;
    };


    void *AeFreeLinkedListAllocator::allocate(std::size_t const t_allocationSize, std::size_t const t_byteAlignment) {

        // If the stored pointer is a nullptr then there is not more memory to give out.
        if (m_firstFreeChunkPtr == nullptr || m_memoryInUse >= m_allocatedMemorySize) {
            throw std::bad_alloc();
        }

        // Return for the allocation.
        return getFirstFit(t_allocationSize, t_byteAlignment);
    };


    void AeFreeLinkedListAllocator::deallocate(void *const t_allocatedMemoryPtr) noexcept {

        // Ensure the address being deallocated is actually controlled by the allocator.
        assert(t_allocatedMemoryPtr > m_allocatedMemoryPtr &&
               t_allocatedMemoryPtr < m_allocatedMemoryTail &&
               "Memory being deallocated not in range of memory this allocator controls!");

        assert(m_memoryInUse > 0 && "Cannot deallocate more memory, currently 0 memory is allocated.");

        // Get back the pointer to the allocated chunk.
        m_allocatedChunk = (AllocatedChunkInfo *) getBaseAddressFromAlignedAddress(t_allocatedMemoryPtr,
                                                                                   sizeof(AllocatedChunkInfo));

        m_fullAllocationSize = m_allocatedChunk->m_chunkSize;

        // Decrement the memory in use for the memory being deallocated.
        m_memoryInUse -= m_fullAllocationSize;

        // Now create a new free chunk from the previously allocated chunk.
        m_newFreeChunk = (FreeChunkInfo *) m_allocatedChunk;

        // Set the new free chunk size to the size of the previously allocated memory
        m_newFreeChunk->m_chunkSize = m_fullAllocationSize;

        // Push the new chunk to the front of the free chunks list.
        m_newFreeChunk->m_nextFreeChunk = m_firstFreeChunkPtr;
        m_firstFreeChunkPtr = m_newFreeChunk;

        // Increment the number of free chunks counter.
        m_numFreeChunks += 1;

        // TODO Create and call the chunk merging function to combine memory chunks that are adjacent.
    };


    void *AeFreeLinkedListAllocator::getBestFit(std::size_t const t_allocationSize,
                                                std::size_t const t_byteAlignment) {

        //TODO implement

        return nullptr;
    };

    // This finds the first free chunk that will fit the desired allocation with the chunk information and still satisfy
    // the alignment requirement. Also deals with splitting any chunk that is bigger than
    void *AeFreeLinkedListAllocator::getFirstFit(std::size_t const t_allocationSize,
                                                 std::size_t const t_byteAlignment) {

        // Initialize the loop variables.
        m_prevFreeChunk = nullptr;
        m_currentFreeChunk = m_firstFreeChunkPtr;

        // TODO Not sure I like this being a while loop, perhaps this should be made into a for look and I should keep
        //  track of the number of free blocks available.
        while (m_currentFreeChunk != nullptr) {

            // Get the pointer for this allocation, use the newFreeChunk
            m_pointerCalculation = getAlignedAddressWithMinimumOffset(m_currentFreeChunk, sizeof(AllocatedChunkInfo),
                                                                      t_byteAlignment);

            // Calculate the full allocation size including the allocation information and the alignment offset.
            m_fullAllocationSize = t_allocationSize +
                                   pointerDifference(m_pointerCalculation, m_currentFreeChunk);

            // See if the allocation
            if (m_currentFreeChunk->m_chunkSize >= m_fullAllocationSize) {

                // Since the chunk can be allocated decrement the number of free chunks counter.
                m_numFreeChunks -= 1;

                // If the chunk is bigger than what we need then we should split it as long as the new chunk resulting
                // from the split is at least big enough to fit the freeChunkInfo.
                if (m_currentFreeChunk->m_chunkSize > m_fullAllocationSize &&
                    m_currentFreeChunk->m_chunkSize - m_fullAllocationSize > sizeof(AllocatedChunkInfo)) {

                    // Create a new chunk by splitting the old one.
                    m_newFreeChunk = (FreeChunkInfo *) addToPointer(m_fullAllocationSize, m_currentFreeChunk);
                    m_newFreeChunk->m_chunkSize = m_currentFreeChunk->m_chunkSize - m_fullAllocationSize;
                    m_newFreeChunk->m_nextFreeChunk = m_currentFreeChunk->m_nextFreeChunk;

                    // Make the previous chunk now point to the new chunk instead of the current one.
                    if (m_prevFreeChunk != nullptr) {
                        m_prevFreeChunk->m_nextFreeChunk = m_newFreeChunk;
                    } else {
                        m_firstFreeChunkPtr = m_newFreeChunk;
                    }

                } else {

                    // Point the previous free chunk to the next free chunk after the current one since the current
                    // chunk is being allocated.
                    if (m_prevFreeChunk != nullptr) {
                        m_prevFreeChunk->m_nextFreeChunk = m_currentFreeChunk->m_nextFreeChunk;
                        m_firstFreeChunkPtr = m_prevFreeChunk;

                    } else if (m_numFreeChunks == 0) {
                        // If there are no more chunks to give out then set the next free chunk pointer to nullptr.
                        m_firstFreeChunkPtr = nullptr;

                    } else {
                        // If there is no previous free chunks and there are more chunks to give out then we must be at
                        // the first free chunk so just set the first free chunk to the next one in the chain.
                        m_firstFreeChunkPtr = m_currentFreeChunk->m_nextFreeChunk;
                    }

                    // If the chunk being allocated is not big enough to split make sure the remainder of the chunk size
                    // is accounted for when setting the chunk size.
                    m_fullAllocationSize = m_currentFreeChunk->m_chunkSize;
                };

                // Allocate the current chunk and create and fill in the newly allocated chunk information.
                m_allocatedChunk = (AllocatedChunkInfo *) m_currentFreeChunk;
                m_allocatedChunk->m_chunkSize = m_fullAllocationSize;
                m_allocatedChunk->m_padding = nullptr;

                // Track the memory being allocated.
                m_memoryInUse += m_fullAllocationSize;

                return getAlignedAddressWithMinimumOffset(m_allocatedChunk,
                                                          sizeof(AllocatedChunkInfo),
                                                          t_byteAlignment);
            };

            // Otherwise the current chunk of memory does not have enough space for the desired allocation then go to
            // the next one.
            m_prevFreeChunk = m_currentFreeChunk;
            m_currentFreeChunk = m_currentFreeChunk->m_nextFreeChunk;
        };

        // If no chunk with a large enough size is found throw and allocation error.
        throw std::bad_alloc();
    };



} //namespace ae_memory