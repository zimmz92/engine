/// \file ae_free_list_allocator.hpp
/// The AeFreeListAllocator class is defined.
#pragma once

// dependencies
#include "ae_allocator_base.hpp"

// libraries

//std
#include <cstdlib>

namespace ae_memory {

    /// A AeFreeListAllocator implements a free list allocator utilizing a linked list.
    class AeFreeLinkedListAllocator: public AeAllocatorBase {

    public:

        /// Constructor of AeStackAllocator.
        AeFreeLinkedListAllocator(std::size_t t_allocatedMemorySize,
                                  void* t_allocatedMemoryPtr) noexcept;

        /// Destructor of the AeStackAllocator.
        ~AeFreeLinkedListAllocator() noexcept override;

        /// Do not allow this class to be copied (2 lines below).
        AeFreeLinkedListAllocator(const AeFreeLinkedListAllocator&) = delete;
        AeFreeLinkedListAllocator& operator=(const AeFreeLinkedListAllocator&) = delete;

        /// Do not allow this class to be moved (2 lines below)
        AeFreeLinkedListAllocator(AeFreeLinkedListAllocator&&) = delete;
        AeFreeLinkedListAllocator& operator=(AeFreeLinkedListAllocator&&) = delete;

        /// Allocates the specified amount of memory.
        /// \param t_allocationSize The number of chunks of the memory to be allocated.
        void* allocate(std::size_t t_allocationSize, std::size_t t_byteAlignment) override;

        /// Deallocates the allocated memory by this allocator at this pointer.
        /// \param t_allocatedMemoryPtr The pointer to the allocated memory which is to be freed.
        void deallocate(void* t_allocatedMemoryPtr) noexcept override;

        /// Implements the equals comparison operator.
        bool operator==(const AeFreeLinkedListAllocator&) const noexcept { return true;};

        /// Implements the not equals comparison operator.
        bool operator!=(const AeFreeLinkedListAllocator&) const noexcept { return false;};

    private:
        /// Stores the information required for a chunk that is part of the free list to keep track of it's own size
        /// and whee the next free chunk of memory is.
        struct FreeChunkInfo{
            size_t m_chunkSize=0;
            FreeChunkInfo* m_nextFreeChunk = nullptr;
        };

        /// Stores the information required for a chunk that that is currently allocated to keep track of it's own size.
        /// The padding is to ensure that when the chunk of memory is deallocated there is enough room to store the
        /// pointer to the next free chunk.
        struct AllocatedChunkInfo{
            size_t m_chunkSize=0;
            FreeChunkInfo* m_padding = nullptr;
        };

        void* getBestFit(std::size_t t_allocationSize, std::size_t t_byteAlignment);

        /// Allocates the first free chunk of memory that will fit the desired allocation. It will attempt to split the
        /// free chunk if it is big enough that the FreeChunkInfo can fit into the new chunk that will be created.
        void* getFirstFit(std::size_t t_allocationSize, std::size_t t_byteAlignment);

        /// Tracks the first free chunk of memory for the allocated memory being managed.
        FreeChunkInfo* m_firstFreeChunkPtr;

        /// A pointer to the top of the allocated memory for quick reference when doing math.
        void* m_allocatedMemoryTail;

        /// Tracks how much of the stack's memory is currently being used.
        std::size_t m_memoryInUse = 0;

        /// Tracks the total number of free blocks available.
        std::size_t m_numFreeChunks = 1;

        /// Used to allocate and split chunks.
        FreeChunkInfo* m_prevFreeChunk;
        FreeChunkInfo* m_currentFreeChunk;
        FreeChunkInfo* m_newFreeChunk;
        void* m_pointerCalculation;

        /// The chunk that is chosen for allocation and needs to be initialized.
        AllocatedChunkInfo* m_allocatedChunk;

        /// Used to calculate the actual size of the allocation including the chunk information and alignment padding.
        std::size_t m_fullAllocationSize;

    protected:

    };
} // namespace ae_memory