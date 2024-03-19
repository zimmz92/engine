/// \file ae_pool_allocator.hpp
/// The AePoolAllocator class is defined.
#pragma once

// dependencies
#include "ae_allocator_base.hpp"

// libraries

//std
#include <cstdlib>

namespace ae_memory {

    /// A AePoolAllocator implements a pool allocator. The allocations from this allocator will always be in multiples
    /// of the pool's memory chuck size.
    class AePoolAllocator: public AeAllocatorBase {
    public:

        /// Constructor of AePoolAllocator.
        AePoolAllocator(std::size_t t_allocatedMemorySize,
                        void* t_allocatedMemoryPtr,
                        std::size_t t_chunkSize,
                        std::size_t t_byteAlignment = MEMORY_ALIGNMENT) noexcept;

        /// Destructor of the AePoolAllocator.
        ~AePoolAllocator() noexcept override;

        /// Do not allow this class to be copied (2 lines below).
        AePoolAllocator(const AePoolAllocator&) = delete;
        AePoolAllocator& operator=(const AePoolAllocator&) = delete;

        /// Do not allow this class to be moved (2 lines below)
        AePoolAllocator(AePoolAllocator&&) = delete;
        AePoolAllocator& operator=(AePoolAllocator&&) = delete;

        /// Allocates the specified amount of memory.
        /// \param t_allocationSize The size of chunk of the memory to be allocated, this must match the size of the
        /// chunks of memory this allocator manages.
        void* allocate(std::size_t t_allocationSize, std::size_t t_byteAlignment) override;

        /// Deallocates the allocated memory by this allocator at this pointer.
        /// \param t_allocatedMemoryPtr The pointer to the allocated memory which is to be freed.
        void deallocate(void* t_allocatedMemoryPtr) noexcept override;

        /// Implements the equals comparison operator.
        bool operator==(const AePoolAllocator&) const noexcept { return true;};

        /// Implements the not equals comparison operator.
        bool operator!=(const AePoolAllocator&) const noexcept { return false;};

    private:

    protected:

        /// Tracks the first free chunk of unit size memory for the allocated memory being managed.
        void* m_firstFreeChunkPtr;

        /// A pointer to the top of the allocated memory for quick reference when doing math for the top-down portion of
        /// the stack.
        void* m_allocatedMemoryTopPtr;

        /// Tracks how much of the stack's memory is currently being used.
        std::size_t m_memoryInUse = 0;

        /// The base size of individual chunks.
        std::size_t m_chunkSize;

        /// The alignment for each chunk.
        std::size_t m_byteAlignment;

        /// The final size of a chunk accounting for the alignment requirements.
        std::size_t m_alignedChunkSize;

        /// A pointer that is used during allocation to store the pointer to the new chunk being allocated.
        void* m_chunkAllocationPtr;
    };
} // namespace ae_memory