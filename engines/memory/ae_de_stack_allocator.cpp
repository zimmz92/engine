/// \file ae_de_stack_allocator.cpp
/// The AeDeStackAllocator class is implemented.
#include "ae_de_stack_allocator.hpp"

// dependencies

// libraries

// std
#include <cassert>

namespace ae_memory {

    AeDeStackAllocator::AeDeStackAllocator(std::size_t const t_allocatedMemorySize, void* const t_allocatedMemoryPtr) noexcept :
    AeAllocatorBase(t_allocatedMemorySize,t_allocatedMemoryPtr){
        // Initialize the bottom-up top of stack pointer.
        m_stackTopPtrBottomUp = t_allocatedMemoryPtr;

        // Set the pointer that tracks where the memory allocated to this allocator ends.
        m_allocatedMemoryTopPtr = addToPointer(t_allocatedMemorySize, t_allocatedMemoryPtr);

        // Initialize the top-down bottom of stack pointer.
        m_stackBottomPtrTopDown = m_allocatedMemoryTopPtr;
    };



    AeDeStackAllocator::~AeDeStackAllocator() noexcept{
        clearDoubleEndedStack();
        m_stackTopPtrBottomUp = nullptr;
        m_stackBottomPtrTopDown = nullptr;
        m_allocatedMemoryTopPtr = nullptr;
    };



    void* AeDeStackAllocator::allocate(std::size_t const t_allocationSize, std::size_t const t_byteAlignment){
        return allocateFromBottom(t_allocationSize,t_byteAlignment);
    };



    void AeDeStackAllocator::deallocate([[maybe_unused]] void* const t_allocatedMemoryPtr) noexcept{
        // Traditional deallocation does not work with the stack allocator.
    };



    void AeDeStackAllocator::clearDoubleEndedStack() noexcept {
        // To clear the entire stack both the top-down and bottom-up portions of the double-ended stack need to be
        // cleared.
        clearTopStack();
        clearBottomStack();
    }



    //==================================================================================================================
    // Bottom-Up Functions
    //==================================================================================================================

    void* AeDeStackAllocator::allocateFromBottom(std::size_t t_allocationSize, std::size_t t_byteAlignment){
        assert(t_allocationSize > 0);

        // Calculate the total memory that will be used, in bytes, to allocate the desired amount of memory and
        // align the memory.
        std::size_t alignmentOffset = getAlignmentOffset(m_stackTopPtrBottomUp, t_byteAlignment);
        std::size_t totalAllocation = alignmentOffset+t_allocationSize;

        // Ensure that this allocation will not exceed the total available memory this stack has available.
        if(totalAllocation + m_memoryInUseBottomUp > m_allocatedMemorySize){
            throw std::bad_alloc();
        }

        // Get the aligned address.
        void* alignedAddress = addToPointer(alignmentOffset, m_stackTopPtrBottomUp);

        // Increment the top of stack pointer by the total allocation.
        m_stackTopPtrBottomUp = addToPointer(totalAllocation, m_stackTopPtrBottomUp);

        // Track the additional memory used.
        m_memoryInUseBottomUp += totalAllocation;

        // Return the aligned address for the allocation.
        return alignedAddress;
    };



    AeDeStackAllocator::BottomStackMarker AeDeStackAllocator::getBottomUpMarker() noexcept{
        return BottomStackMarker{m_stackTopPtrBottomUp};
    };



    void AeDeStackAllocator::deallocateToBottomMarker(AeDeStackAllocator::BottomStackMarker t_marker) noexcept {
        // Ensure to only deallocate memory that this allocator controls.
        assert(t_marker.m_ptr > m_allocatedMemoryPtr);

        // Decrement the amount of memory in use due to the deallocation.
        m_memoryInUseBottomUp = m_memoryInUseBottomUp - pointerDifference(m_stackTopPtrBottomUp, t_marker.m_ptr);

        // Roll the top of stack pointer back to the provided marker location.
        m_stackTopPtrBottomUp = t_marker.m_ptr;
    }



    void AeDeStackAllocator::clearBottomStack() noexcept{
        // Roll the bottom-up top of stack pointer all the way back to the start of the memory allocated for the stack
        // itself.
        m_stackTopPtrBottomUp = m_allocatedMemoryPtr;
        m_memoryInUseBottomUp = 0;
    };



    //==================================================================================================================
    // Top-Down Functions
    //==================================================================================================================

    AeDeStackAllocator::TopStackMarker AeDeStackAllocator::getTopMarker() noexcept{
        return TopStackMarker{m_stackBottomPtrTopDown};
    };



    void* AeDeStackAllocator::allocateFromTop(std::size_t t_allocationSize, std::size_t t_byteAlignment){

    };



    void AeDeStackAllocator::deallocateToTopMarker(AeDeStackAllocator::TopStackMarker t_marker) noexcept {
        // Ensure to only deallocate memory that this allocator controls.
        assert(t_marker.m_ptr < m_allocatedMemoryTopPtr);

        // Decrement the amount of memory in use due to the deallocation.
        m_memoryInUseBottomUp = m_memoryInUseBottomUp - pointerDifference(t_marker.m_ptr, m_stackBottomPtrTopDown);

        // Roll the top of stack pointer back to the provided marker location.
        m_stackBottomPtrTopDown = t_marker.m_ptr;
    }



    void AeDeStackAllocator::clearTopStack() noexcept{
        // Roll the top-down bottom of stack pointer all the way back up to the end of the memory allocated for the
        // stack itself.
        m_stackBottomPtrTopDown = m_allocatedMemoryTopPtr;
        m_memoryInUseTopDown = 0;
    };

} //namespace ae_memory