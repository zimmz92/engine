/// \file ae_memory_manager.hpp
/// The AeMemoryManager class is defined.
#pragma once

// dependencies
#include "ae_allocator_base.hpp"

// libraries

// std
#include <cstdlib>
#include <forward_list>

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

        /// A list of which allocators are being managed.
        std::forward_list<AeAllocatorBase*> m_managedAllocators;

        /// The amount of memory allocated to this manager.
        std::size_t m_memoryBlockSize;
    };

} // namespace ae