/// \file ae_de_stack_allocator.cpp
/// The AeStackAllocator class is implemented.
#include "ae_de_stack_allocator.hpp"

// dependencies

// libraries

// std
#include <cassert>

namespace ae_memory {

    AeDeStackAllocator::AeDeStackAllocator(std::size_t const t_allocatedMemorySize, void* const t_allocatedMemoryPtr) noexcept :
    AeAllocatorBase(t_allocatedMemorySize,t_allocatedMemoryPtr){
        m_stackTopPtrBottomUp = t_allocatedMemoryPtr;
    };



    AeDeStackAllocator::~AeDeStackAllocator() noexcept{
        clearStack();
        m_stackTopPtrBottomUp = nullptr;
    };



    void* AeDeStackAllocator::allocate(std::size_t const t_allocationSize, std::size_t const t_byteAlignment){
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



    void AeDeStackAllocator::deallocate([[maybe_unused]] void* const t_allocatedMemoryPtr) noexcept{
        // Traditional deallocation does not work with the stack allocator.
    };



    void AeDeStackAllocator::clearStack() noexcept {
        // Roll the top of stack pointer all the way back to the start of the memory allocated for the stack itself.
        m_stackTopPtrBottomUp = m_allocatedMemoryPtr;
        m_memoryInUseBottomUp = 0;
    }



    //==================================================================================================================
    // Bottom-Up Functions
    //==================================================================================================================

    AeDeStackAllocator::BottomUpStackMarker AeDeStackAllocator::getBottomUpMarker() noexcept{
        return BottomUpStackMarker{m_stackTopPtrBottomUp};
    };



    void AeDeStackAllocator::deallocateToMarkerBottomUp(AeDeStackAllocator::BottomUpStackMarker t_marker) noexcept {
        // Ensure to only deallocate memory that this allocator controls.
        assert(t_marker.m_ptr > m_allocatedMemoryPtr);

        // Decrement the amount of memory in use due to the deallocation.
        m_memoryInUseBottomUp = m_memoryInUseBottomUp - pointerDifference(m_stackTopPtrBottomUp, t_marker.m_ptr);

        // Roll the top of stack pointer back to the provided marker location.
        m_stackTopPtrBottomUp = t_marker.m_ptr;
    }



    void AeDeStackAllocator::clearBottomUp() noexcept{

    };



    //==================================================================================================================
    // Top-Down Functions
    //==================================================================================================================

    AeDeStackAllocator::TopDownStackMarker AeDeStackAllocator::getTopDownMarker() noexcept{
        return TopDownStackMarker{m_stackBottomPtrTopDown};
    };



    void AeDeStackAllocator::deallocateToMarkerTopDown(AeDeStackAllocator::TopDownStackMarker t_marker) noexcept {
        // Ensure to only deallocate memory that this allocator controls.
        assert(t_marker.m_ptr > m_allocatedMemoryPtr);

        // Decrement the amount of memory in use due to the deallocation.
        m_memoryInUseBottomUp = m_memoryInUseBottomUp - pointerDifference(m_stackTopPtrBottomUp, t_marker.m_ptr);

        // Roll the top of stack pointer back to the provided marker location.
        m_stackTopPtrBottomUp = t_marker.m_ptr;
    }



    void AeDeStackAllocator::clearTopDown() noexcept{

    };

} //namespace ae_memory