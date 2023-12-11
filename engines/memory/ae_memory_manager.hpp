/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_allocator_base.hpp"

// libraries

// std
#include <cstdlib>
#include <unordered_set>

namespace ae_memory {

    /// Allocates and manages the block of memory which the game will utilize.
    class AeMemoryManager{
    public:

        /// Creates a new memory manager that will allocated, and manage, a new block of memory as specified. This will
        /// be responsible for providing allocators with blocks of the pre-allocated memory to give out as requested.
        /// \param t_memoryBlockSize The size of the block of memory to allocate and manage.
        explicit AeMemoryManager(std::size_t t_memoryBlockSize);

        /// First cleans up the allocators that utilized the managed memory block, then frees the memory block.
        ~AeMemoryManager();

    private:
    protected:

        /// The base pointer to the block of allocated memory.
        void* m_allocatedMemoryPtr;

        /// A
        std::unordered_set<AeAllocatorBase*> m_managedAllocators;

        ///
        std::size_t m_memoryBlockSize;
    };

} // namespace ae