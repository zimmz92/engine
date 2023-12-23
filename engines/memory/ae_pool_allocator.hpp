/// \file ae_pool_allocator.hpp
/// The AePoolAllocator class is defined.
#pragma once

// dependencies
#include "ae_allocator_base.hpp"

// libraries

//std
#include <cstdlib>

namespace ae_memory {

    /// A AeStackAllocator implements a stack allocator. Memory will always be allocated from the top of the stack.
    /// Deallocation may only be done by passing back a pointer to where
    class AePoolAllocator: public AeAllocatorBase {
    public:

        /// This is used to ensure that only a retrieved marker can be used when calling deallocateToMarker to help
        /// enforce that a returned pointer for allocation is not suitable for deallocation since it will not account
        /// for any alignment requirements.
        typedef struct{void* m_ptr;} StackMarker;

        /// Constructor of AeStackAllocator.
        AePoolAllocator(std::size_t t_allocatedMemorySize,
                        void* t_allocatedMemoryPtr,
                        std::size_t t_chunkSize,
                        std::size_t t_byteAlignment = MEMORY_ALIGNMENT) noexcept;

        /// Destructor of the AeStackAllocator.
        ~AePoolAllocator() noexcept override;

        /// Do not allow this class to be copied (2 lines below).
        AePoolAllocator(const AePoolAllocator&) = delete;
        AePoolAllocator& operator=(const AePoolAllocator&) = delete;

        /// Do not allow this class to be moved (2 lines below)
        AePoolAllocator(AePoolAllocator&&) = delete;
        AePoolAllocator& operator=(AePoolAllocator&&) = delete;

        /// Allocates the specified amount of memory.
        /// \param t_allocationSize The number of chunks of the memory to be allocated.
        /// \param t_byteAlignment Unused by this allocator.
        void* allocate(std::size_t t_allocationSize, [[maybe_unused]] std::size_t t_byteAlignment) override;

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

        /// Tracks how much of the stack's memory is currently being used.
        std::size_t m_memoryInUse = 0;

        std::size_t m_chunkSize;

        std::size_t m_byteAlignment;
    };
} // namespace ae_memory