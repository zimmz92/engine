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
        m_bottomStackPtr = t_allocatedMemoryPtr;

        // Set the pointer that tracks where the memory allocated to this allocator ends.
        m_allocatedMemoryTopPtr = addToPointer(t_allocatedMemorySize, t_allocatedMemoryPtr);

        // Initialize the top-down bottom of stack pointer.
        m_topStackPtr = m_allocatedMemoryTopPtr;
    };



    AeDeStackAllocator::~AeDeStackAllocator() noexcept{
        clearDoubleEndedStack();
        m_bottomStackPtr = nullptr;
        m_topStackPtr = nullptr;
        m_allocatedMemoryTopPtr = nullptr;
    };



    void* AeDeStackAllocator::allocate(std::size_t const t_allocationSize, std::size_t const t_byteAlignment){
        // Allocation from the top stack shall be the default since the bottom portion of the stack will be used for
        // longer term memory allocations where the top stack will be used for shorter term allocation.
        return allocateFromTop(t_allocationSize,t_byteAlignment);
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
    // Bottom Stack Functions
    //==================================================================================================================

    void* AeDeStackAllocator::allocateFromBottom(std::size_t t_allocationSize, std::size_t t_byteAlignment){
        assert(t_allocationSize > 0);

        // Calculate the total memory that will be used, in bytes, to allocate the desired amount of memory and
        // align the memory.
        std::size_t alignmentOffset = getAlignmentOffset(m_bottomStackPtr, t_byteAlignment);
        std::size_t totalAllocation = alignmentOffset+t_allocationSize;

        // Ensure that this allocation will not exceed the total available memory this stack has available.
        if(totalAllocation + m_bottomStackMemoryUsage + m_topStackMemoryUsage > m_allocatedMemorySize){
            throw std::bad_alloc();
        }

        // Get the aligned address.
        void* alignedAddress = addToPointer(alignmentOffset, m_bottomStackPtr);

        // Increment the top of stack pointer by the total allocation.
        m_bottomStackPtr = addToPointer(totalAllocation, m_bottomStackPtr);

        // Track the additional memory used.
        m_bottomStackMemoryUsage += totalAllocation;

        // Return the aligned address for the allocation.
        return alignedAddress;
    };



    AeDeStackAllocator::BottomStackMarker AeDeStackAllocator::getBottomStackMarker() noexcept{
        return BottomStackMarker{m_bottomStackPtr};
    };



    void AeDeStackAllocator::deallocateToBottomMarker(AeDeStackAllocator::BottomStackMarker t_marker) noexcept {
        // Ensure to only deallocate memory that this allocator controls.
        assert(t_marker.m_ptr >= m_allocatedMemoryPtr);

        // Decrement the amount of memory in use due to the deallocation.
        m_bottomStackMemoryUsage = m_bottomStackMemoryUsage - pointerDifference(m_bottomStackPtr, t_marker.m_ptr);

        // Roll the top of stack pointer back to the provided marker location.
        m_bottomStackPtr = t_marker.m_ptr;
    }



    void AeDeStackAllocator::clearBottomStack() noexcept{
        // Roll the bottom-up top of stack pointer all the way back to the start of the memory allocated for the stack
        // itself.
        m_bottomStackPtr = m_allocatedMemoryPtr;
        m_bottomStackMemoryUsage = 0;
    };



    //==================================================================================================================
    // Top Stack Functions
    //==================================================================================================================

    AeDeStackAllocator::TopStackMarker AeDeStackAllocator::getTopStackMarker() noexcept{
        return TopStackMarker{m_topStackPtr};
    };




    void* AeDeStackAllocator::allocateFromTop(std::size_t t_allocationSize, std::size_t t_byteAlignment){
        assert(t_allocationSize > 0);

        // Calculate the address with the allocation before alignment.
        void* unalignedAddress = subtractFromPointer(t_allocationSize, m_topStackPtr);

        // Calculate the required offset to align the memory.
        // address = 0110
        // alignment = 0100
        // 0110 - 0100 = 0010
        // 0010 & (0100-0001) = 0010 & 0011 = 0010
        // (address - alignment) & ~(alignment-1)
        std::size_t alignmentOffset = (reinterpret_cast<std::size_t>(unalignedAddress) - t_byteAlignment)  & (t_byteAlignment-1);

        // Calculate the total memory that will be used, in bytes, to allocate the desired amount of memory and
        // align the memory.
        std::size_t totalAllocation = alignmentOffset+t_allocationSize;

        // Ensure that this allocation will not exceed the total available memory this stack has available.
        if(totalAllocation + m_bottomStackMemoryUsage + m_topStackMemoryUsage > m_allocatedMemorySize){
            throw std::bad_alloc();
        };

        // The bottom of stack pointer is the same as the aligned address in the case of the top-down portion of the
        // stack.
        m_topStackPtr = subtractFromPointer(alignmentOffset, unalignedAddress);

        // Track the additional memory used.
        m_topStackMemoryUsage += totalAllocation;

        // Return the aligned address for the allocation.
        return m_topStackPtr;
    };



    void AeDeStackAllocator::deallocateToTopMarker(AeDeStackAllocator::TopStackMarker t_marker) noexcept {
        // Ensure to only deallocate memory that this allocator controls.
        assert(t_marker.m_ptr <= m_allocatedMemoryTopPtr);

        // Decrement the amount of memory in use due to the deallocation.
        m_topStackMemoryUsage = m_topStackMemoryUsage - pointerDifference(t_marker.m_ptr,m_topStackPtr);

        // Roll the top of stack pointer back to the provided marker location.
        m_topStackPtr = t_marker.m_ptr;
    }



    void AeDeStackAllocator::clearTopStack() noexcept{
        // Roll the top-down bottom of stack pointer all the way back up to the end of the memory allocated for the
        // stack itself.
        m_topStackPtr = m_allocatedMemoryTopPtr;
        m_topStackMemoryUsage = 0;
    };

} //namespace ae_memory