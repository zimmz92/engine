/// \file ae_allocator.hpp
/// The AeStackAllocator class is defined.
#pragma once

// dependencies
#include "ae_allocator_base.hpp"

// libraries

//std
#include <cstdlib>

namespace ae_memory {

    /// A AeStackAllocator implements a stack allocator. Memory will always be allocated from the top of the stack.
    /// Deallocation may only be done by passing back a pointer to where
    class AeStackAllocator: public AeAllocatorBase {
    public:

        /// This is used to ensure that only a retrieved marker can be used when calling deallocateToMarker to help
        /// enforce that a returned pointer for allocation is not suitable for deallocation since it will not account
        /// for any alignment requirements.
        typedef struct{void* m_ptr;} StackMarker;

        /// Constructor of AeStackAllocator.
        AeStackAllocator(std::size_t t_allocatedMemorySize, void* t_allocatedMemoryPtr) noexcept;

        /// Destructor of the AeStackAllocator.
        ~AeStackAllocator() noexcept override;

        /// Do not allow this class to be copied (2 lines below).
        AeStackAllocator(const AeStackAllocator&) = delete;
        AeStackAllocator& operator=(const AeStackAllocator&) = delete;

        /// Do not allow this class to be moved (2 lines below)
        AeStackAllocator(AeStackAllocator&&) = delete;
        AeStackAllocator& operator=(AeStackAllocator&&) = delete;

        /// Allocates the specified amount of memory.
        /// \param t_allocationSize The size of the memory in bytes to be allocated.
        void* allocate(std::size_t t_allocationSize, std::size_t t_byteAlignment) override;

        /// Deallocates the allocated memory by this allocator at this pointer.
        /// \param t_allocatedMemoryPtr The pointer to the allocated memory which is to be freed.
        void deallocate(void* t_allocatedMemoryPtr) noexcept override;

        /// Returns a marker which represents the current top of the stack. A marker must be used to free memory from
        /// the stack allocator.
        StackMarker getMarker() noexcept;

        /// Deallocates all memory from the current top of the stack to the provided marker.
        /// \param t_marker The marker representing the memory location that the stack shall be
        /// rolled back to.
        void deallocateToMarker(StackMarker t_marker) noexcept;

        /// Deallocates all the memory in the stack.
        void clearStack() noexcept;

        /// Implements the equals comparison operator.
        bool operator==(const AeStackAllocator&) const noexcept { return true;};

        /// Implements the not equals comparison operator.
        bool operator!=(const AeStackAllocator&) const noexcept { return false;};

    private:

    protected:

        /// Tracks the current top of the stack of memory being managed.
        void* m_stackTopPtr;

        /// Tracks how much of the stack's memory is currently being used.
        std::size_t m_memoryInUse = 0;
    };
} // namespace ae_memory