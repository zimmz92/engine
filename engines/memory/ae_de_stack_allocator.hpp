/// \file ae_de_stack_allocator.hpp
/// The AeDeStackAllocator class is defined.
#pragma once

// dependencies
#include "ae_allocator_base.hpp"

// libraries

//std
#include <cstdlib>

namespace ae_memory {

    /// A AeDeStackAllocator implements a double-ended stack allocator. Memory can be allocated in a bottom-up fashion as
    /// a typical stack would (default when allocate is called) or in a top-down fashion. Deallocation may only be done
    /// by passing back either a BottomUpStackMarker, if deallocating memory from the bottom-up portion of the stack, or
    /// a TopDownStackMarker if deallocating memory from the top-down portion of the stack.
    class AeDeStackAllocator: public AeAllocatorBase {
    public:

        /// This is used to ensure that only a retrieved marker can be used when calling deallocateToMarkerBottomUp to
        /// help enforce that a returned pointer for allocation is not suitable for deallocation since it will not
        /// account for any alignment requirements.
        typedef struct{void* m_ptr;} BottomStackMarker;

        /// This is used to ensure that only a retrieved marker can be used when calling deallocateToMarkerTopDown to
        /// help enforce that a returned pointer for allocation is not suitable for deallocation since it will not
        /// account for any alignment requirements.
        typedef struct{void* m_ptr;} TopStackMarker;

        /// Constructor of AeStackAllocator.
        AeDeStackAllocator(std::size_t t_allocatedMemorySize, void* t_allocatedMemoryPtr) noexcept;

        /// Destructor of the AeStackAllocator.
        ~AeDeStackAllocator() noexcept override;

        /// Do not allow this class to be copied (2 lines below).
        AeDeStackAllocator(const AeDeStackAllocator&) = delete;
        AeDeStackAllocator& operator=(const AeDeStackAllocator&) = delete;

        /// Do not allow this class to be moved (2 lines below)
        AeDeStackAllocator(AeDeStackAllocator&&) = delete;
        AeDeStackAllocator& operator=(AeDeStackAllocator&&) = delete;

        /// Allocates the specified amount of memory.
        /// \param t_allocationSize The size of the memory in bytes to be allocated.
        void* allocate(std::size_t t_allocationSize, std::size_t t_byteAlignment) override;

        /// Deallocates the allocated memory by this allocator at this pointer.
        /// \param t_allocatedMemoryPtr The pointer to the allocated memory which is to be freed.
        void deallocate(void* t_allocatedMemoryPtr) noexcept override;

        /// Deallocates all the memory in the stack.
        void clearDoubleEndedStack() noexcept;

        /// Implements the equals comparison operator.
        bool operator==(const AeDeStackAllocator&) const noexcept { return true;};

        /// Implements the not equals comparison operator.
        bool operator!=(const AeDeStackAllocator&) const noexcept { return false;};

        //==================================================================================================================
        // Bottom Stack Functions
        //==================================================================================================================

        /// Returns a marker which represents the current top of the stack. A marker must be used to free memory from
        /// the stack allocator.
        BottomStackMarker getBottomStackMarker() noexcept;

        /// Allocates the specified amount of memory from the bottom portion of the stack.
        /// \param t_allocationSize The size of the memory in bytes to be allocated.
        void* allocateFromBottom(std::size_t t_allocationSize, std::size_t t_byteAlignment=MEMORY_ALIGNMENT);

        /// Deallocates all memory from the current top of the bottom-up portion of the stack to the provided marker.
        /// \param t_marker The marker representing the memory location that the bottom-up portion of the stack shall be
        /// rolled down to.
        void deallocateToBottomMarker(BottomStackMarker t_marker) noexcept;

        /// Deallocates only the bottom-up portion of the stack.
        void clearBottomStack() noexcept;


        //==================================================================================================================
        // Top Stack Functions
        //==================================================================================================================

        /// Returns a marker which represents the current top of the stack. A marker must be used to free memory from
        /// the stack allocator.
        TopStackMarker getTopStackMarker() noexcept;

        /// Allocates the specified amount of memory from the top portion of the stack.
        /// \param t_allocationSize The size of the memory in bytes to be allocated.
        void* allocateFromTop(std::size_t t_allocationSize, std::size_t t_byteAlignment=MEMORY_ALIGNMENT);

        /// Deallocates all memory from the current "top", bottom, of the top-down portion of the stack to the provided
        /// marker.
        /// \param t_marker The marker representing the memory location that the top-down portion of the stack shall be
        /// rolled up to.
        void deallocateToTopMarker(TopStackMarker t_marker) noexcept;

        /// Deallocates only the top-down portion of the stack.
        void clearTopStack() noexcept;



    private:

    protected:

        /// Tracks the current top of the stack for the bottom-up portion of the stack memory being managed.
        void* m_bottomStackPtr;

        /// Tracks the current "top", bottom, of the stack for the top-down portion of the stack memory being managed.
        void* m_topStackPtr;

        /// Tracks how much of the stack's memory is currently being used by the bottom-up portion.
        std::size_t m_bottomStackMemoryUsage = 0;

        /// Tracks how much of the stack's memory is currently being used by the top-down portion.
        std::size_t m_topStackMemoryUsage = 0;

        /// A pointer to the top of the allocated memory for quick reference when doing math for the top-down portion of
        /// the stack.
        void* m_allocatedMemoryTopPtr;
    };
} // namespace ae_memory