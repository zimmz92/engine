/// \file ae_pool_allocator.cpp
/// The AePoolAllocator class is implemented.
#include "ae_pool_allocator.hpp"

// dependencies

// libraries

// std
#include <cassert>

namespace ae_memory {

    AePoolAllocator::AePoolAllocator(std::size_t const t_allocatedMemorySize, void* const t_allocatedMemoryPtr) noexcept :
    AeAllocatorBase(t_allocatedMemorySize,t_allocatedMemoryPtr){
        m_stackTopPtr = t_allocatedMemoryPtr;
    };



    AePoolAllocator::~AePoolAllocator() noexcept{
        clearStack();
        m_stackTopPtr = nullptr;
    };


    void* AePoolAllocator::allocate(std::size_t const t_allocationSize, std::size_t const t_byteAlignment){
        assert(t_allocationSize > 0);

        // Calculate the total memory that will be used, in bytes, to allocate the desired amount of memory and
        // align the memory.
        std::size_t alignmentOffset = getAlignmentOffset(m_stackTopPtr, t_byteAlignment);
        std::size_t totalAllocation = alignmentOffset+t_allocationSize;

        // Ensure that this allocation will not exceed the total available memory this stack has available.
        if(totalAllocation+m_memoryInUse>m_allocatedMemorySize){
            throw std::bad_alloc();
        }

        // Get the aligned address.
        void* alignedAddress = addToPointer(alignmentOffset,m_stackTopPtr);

        // Increment the top of stack pointer by the total allocation.
        m_stackTopPtr = addToPointer(totalAllocation,m_stackTopPtr);

        // Track the additional memory used.
        m_memoryInUse += totalAllocation;

        // Return the aligned address for the allocation.
        return alignedAddress;
    };


    void AePoolAllocator::deallocate([[maybe_unused]] void* const t_allocatedMemoryPtr) noexcept{
        // Traditional deallocation does not work with the stack allocator.
    };

    AePoolAllocator::StackMarker AePoolAllocator::getMarker() noexcept{
        return StackMarker{m_stackTopPtr};
    };

    void AePoolAllocator::deallocateToMarker(AePoolAllocator::StackMarker t_marker) noexcept {
        // Ensure to only deallocate memory that this allocator controls.
        assert(t_marker.m_ptr > m_allocatedMemoryPtr);

        // Decrement the amount of memory in use due to the deallocation.
        m_memoryInUse = m_memoryInUse - pointerDifference(m_stackTopPtr,t_marker.m_ptr);

        // Roll the top of stack pointer back to the provided marker location.
        m_stackTopPtr = t_marker.m_ptr;
    }

    void AePoolAllocator::clearStack() noexcept {
        // Roll the top of stack pointer all the way back to the start of the memory allocated for the stack itself.
        m_stackTopPtr = m_allocatedMemoryPtr;
        m_memoryInUse = 0;
    }
} //namespace ae_memory