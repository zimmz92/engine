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

        assert(t_allocatedMemorySize>sizeof(FreeChunkInfo) && "Memory allocated to AeFreeLinkedListAllocator is not"
                                                              " big enough for the FreeChunkInfo struct.");

        // Calculate the top of the range of memory that was allocated. This gets used to ensure this allocator does not
        // attempt to manage memory that does not belong to it.
        m_allocatedMemoryTail = addToPointer(m_allocatedMemorySize,m_allocatedMemoryPtr);

        // Create the initial free chunk which is just the entire memory that has been given to this allocator to
        // manage.
        m_firstFreeChunkPtr = static_cast<FreeChunkInfo*>(t_allocatedMemoryPtr);
        m_firstFreeChunkPtr->m_chunkSize = t_allocatedMemorySize;
        m_firstFreeChunkPtr->m_nextFreeChunk = nullptr;
    };


    AeFreeLinkedListAllocator::~AeFreeLinkedListAllocator() noexcept {
        assert(m_memoryInUse==0 && "Huston we have a leak... in a pool... allocator!");

        m_firstFreeChunkPtr = nullptr;
        m_allocatedMemoryTail = nullptr;
    };


    void *AeFreeLinkedListAllocator::allocate(std::size_t const t_allocationSize, std::size_t const t_byteAlignment) {

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


    void AeFreeLinkedListAllocator::deallocate(void* const t_allocatedMemoryPtr) noexcept {

        // Ensure the address being deallocated is actually controlled by the allocator.
        assert(t_allocatedMemoryPtr > m_allocatedMemoryPtr &&
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


    void* AeFreeLinkedListAllocator::getBestFit(std::size_t const t_allocationSize,
                                                std::size_t const t_byteAlignment){

    };

    void* AeFreeLinkedListAllocator::getFirstFit(std::size_t const t_allocationSize,
                                                 std::size_t const t_byteAlignment){

        // Initialize the loop variables.
        m_prevFreeChunk = nullptr;
        m_currentFreeChunk = m_firstFreeChunkPtr;

        // TODO Not sure I like this being a while loop, perhaps this should be made into a for look and I should keep
        //  track of the number of free blocks available.
        while(m_currentFreeChunk!= nullptr){

            // Calculate the full allocation size including the allocation information and the alignment offset.
            m_fullAllocationSize = getAlignmentOffset(addToPointer(sizeof(AllocatedChunkInfo),m_currentFreeChunk),t_byteAlignment);


            if(m_currentFreeChunk->m_chunkSize >= m_fullAllocationSize) {

                // If the chunk is bigger than what we need then we should split it as long as the new chunk resulting
                // from the split is at least big enough to fit the freeChunkInfo.
                if (m_currentFreeChunk->m_chunkSize > m_fullAllocationSize &&
                    m_currentFreeChunk->m_chunkSize - m_fullAllocationSize > sizeof(AllocatedChunkInfo)) {

                    // Create a new chunk by splitting the old one.
                    m_newFreeChunk = (FreeChunkInfo *) addToPointer(m_fullAllocationSize, m_currentFreeChunk);
                    m_newFreeChunk->m_chunkSize = m_currentFreeChunk->m_chunkSize - m_fullAllocationSize;
                    m_newFreeChunk->m_nextFreeChunk = m_currentFreeChunk->m_nextFreeChunk;

                    // Make the previous chunk now point to the new chunk instead of the current one.
                    m_prevFreeChunk->m_nextFreeChunk = m_newFreeChunk;

                } else {
                    // Point the previous free chunk to the next free chunk after the current one since the current
                    // chunk is being allocated.
                    if (m_prevFreeChunk != nullptr) {
                        m_prevFreeChunk->m_nextFreeChunk = m_currentFreeChunk->m_nextFreeChunk;
                    }
                };

                // Allocate the current chunk and create and fill in the newly allocated chunk information.
                m_allocatedChunk = (AllocatedChunkInfo *) m_currentFreeChunk;
                m_allocatedChunk->m_chunkSize = t_allocationSize;
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