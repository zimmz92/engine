/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_allocator_base.hpp"
#include "ae_stack_allocator.hpp"
#include "ae_de_stack_allocator.hpp"

// libraries

// std


namespace ae {

    void test_memory_pointer_functions(){

        // Get a random new pointer with a bit of a buffer. Not going to do anything to the memory but don't want to
        // cause other issues.
        void* base_ptr = malloc(sizeof(std::uint8_t)*64);

        // Offset the pointer so the alignment math shows itself.
        auto* starting_ptr = (void*)&(reinterpret_cast<std::uint8_t*>(base_ptr)[2]);

        void* alignedAddress = ae_memory::AeAllocatorBase::getAlignedAddress(starting_ptr);

        std::size_t alignmentOffset = ae_memory::AeAllocatorBase::getAlignmentOffset(starting_ptr);


        void* alignedAddressWStoredOffset = ae_memory::AeAllocatorBase::getAlignedAddressWithStoredOffset(starting_ptr);
        auto storedAlignmentOffset = static_cast<std::size_t>(reinterpret_cast<std::uint8_t*>(alignedAddressWStoredOffset)[-1]);
        void* baseAddressFromStoredOffset = ae_memory::AeAllocatorBase::getBaseAddressFromAlignedAddress(alignedAddressWStoredOffset);

        void* alignedAddressWMinimumOffset = ae_memory::AeAllocatorBase::getAlignedAddressWithMinimumOffset(starting_ptr,sizeof(std::uint8_t)*3);
        auto storedAlignmentOffsetMinimumOffset = static_cast<std::size_t>(reinterpret_cast<std::uint8_t*>(alignedAddressWMinimumOffset)[-1]);
        void* baseAddressFromMinOffset = ae_memory::AeAllocatorBase::getBaseAddressFromAlignedAddress(alignedAddressWMinimumOffset,sizeof(std::uint8_t)*3);

        void* alignedAddressWVoidOffset = ae_memory::AeAllocatorBase::getAlignedAddressWithVoidPtrOffsets(starting_ptr,2);
        auto storedAlignmentOffsetVoidOffset = static_cast<std::size_t>(reinterpret_cast<std::uint8_t*>(alignedAddressWVoidOffset)[-1]);
        void* baseAddressFromVoid = ae_memory::AeAllocatorBase::getBaseAddressFromAlignedAddress(alignedAddressWVoidOffset,sizeof(void*)*2);

        free(base_ptr);

    };

    void test_stack_allocator(){
        // In bytes.
        std::size_t preAllocatedSize = 128;
        void* preAllocatedMemoryPtr = malloc(preAllocatedSize);

        auto* testStackAllocator = new ae_memory::AeStackAllocator(preAllocatedSize,preAllocatedMemoryPtr);


        // Test that the stack allocator properly gives out memory and the proper stack markers.
        int* testIntegerArrayA = (int*)testStackAllocator->allocate(sizeof(int)*5,sizeof(int));
        ae_memory::AeStackAllocator::StackMarker postAMarker = testStackAllocator->getMarker();
        int* testIntegerArrayB = (int*)testStackAllocator->allocate(sizeof(int)*6,sizeof(int));
        ae_memory::AeStackAllocator::StackMarker postBMarker = testStackAllocator->getMarker();
        int* testIntegerArrayC = (int*)testStackAllocator->allocate(sizeof(int)*3,sizeof(int));

        // Ensure deallocation to markers works properly.
        testStackAllocator->deallocateToMarker(postBMarker);
        testStackAllocator->deallocateToMarker(postAMarker);

        // Check to see if clearing the stack works.
        testIntegerArrayC = (int*)testStackAllocator->allocate(sizeof(int)*7,sizeof(int));
        testStackAllocator->clearStack();

        // Ensure that the stack will throw an error as expected if too much memory is being allocated from it.
        testIntegerArrayA = (int*)testStackAllocator->allocate(sizeof(int)*30,sizeof(int));
        testIntegerArrayB = (int*)testStackAllocator->allocate(sizeof(int)*2,sizeof(int));
        testIntegerArrayC = (int*)testStackAllocator->allocate(sizeof(int)*3,sizeof(int));

        delete(testStackAllocator);
        testStackAllocator = nullptr;

        free(preAllocatedMemoryPtr);
        preAllocatedMemoryPtr = nullptr;
    }

    void test_de_stack_allocator(){
        // In bytes.
        std::size_t preAllocatedSize = 128;
        void* preAllocatedMemoryPtr = malloc(preAllocatedSize);

        auto* testStackAllocator = new ae_memory::AeDeStackAllocator(preAllocatedSize,preAllocatedMemoryPtr);

        //==============================================================================================================
        // Test the bottom stack
        //==============================================================================================================

        // Test that the stack allocator properly gives out memory and the proper stack markers.
        int* testIntegerArrayA = (int*)testStackAllocator->allocateFromBottom(sizeof(int)*5,sizeof(int));
        ae_memory::AeDeStackAllocator::BottomStackMarker postAMarker = testStackAllocator->getBottomStackMarker();
        int* testIntegerArrayB = (int*)testStackAllocator->allocateFromBottom(sizeof(int)*6,sizeof(int));
        ae_memory::AeDeStackAllocator::BottomStackMarker postBMarker = testStackAllocator->getBottomStackMarker();
        int* testIntegerArrayC = (int*)testStackAllocator->allocateFromBottom(sizeof(int)*3,sizeof(int));

        // Ensure deallocation to markers works properly.
        testStackAllocator->deallocateToBottomMarker(postBMarker);
        testStackAllocator->deallocateToBottomMarker(postAMarker);

        // Check to see if clearing the stack works.
        testIntegerArrayC = (int*)testStackAllocator->allocateFromBottom(sizeof(int)*7,sizeof(int));
        testStackAllocator->clearBottomStack();

        // Ensure that the stack will throw an error as expected if too much memory is being allocated from it.
        //testIntegerArrayA = (int*)testStackAllocator->allocateFromBottom(sizeof(int)*30,sizeof(int));
        //testIntegerArrayB = (int*)testStackAllocator->allocateFromBottom(sizeof(int)*2,sizeof(int));
        //testIntegerArrayC = (int*)testStackAllocator->allocateFromBottom(sizeof(int)*3,sizeof(int));

        //==============================================================================================================
        // Test the top stack
        //==============================================================================================================
        // Test that the stack allocator properly gives out memory and the proper stack markers.
        testIntegerArrayA = (int*)testStackAllocator->allocateFromTop(sizeof(int)*5,sizeof(int));
        ae_memory::AeDeStackAllocator::TopStackMarker postATopMarker = testStackAllocator->getTopStackMarker();
        testIntegerArrayB = (int*)testStackAllocator->allocateFromTop(sizeof(int)*6,sizeof(int));
        ae_memory::AeDeStackAllocator::TopStackMarker postBTopMarker = testStackAllocator->getTopStackMarker();
        testIntegerArrayC = (int*)testStackAllocator->allocateFromTop(sizeof(int)*3,sizeof(int));

        // Ensure deallocation to markers works properly.
        testStackAllocator->deallocateToTopMarker(postBTopMarker);
        testStackAllocator->deallocateToTopMarker(postATopMarker);

        // Check to see if clearing the stack works.
        testIntegerArrayC = (int*)testStackAllocator->allocateFromTop(sizeof(int)*7,sizeof(int));
        testStackAllocator->clearTopStack();

        // Ensure that the stack will throw an error as expected if too much memory is being allocated from it.
        //testIntegerArrayA = (int*)testStackAllocator->allocateFromTop(sizeof(int)*30,sizeof(int));
        //testIntegerArrayB = (int*)testStackAllocator->allocateFromTop(sizeof(int)*2,sizeof(int));
        //testIntegerArrayC = (int*)testStackAllocator->allocateFromTop(sizeof(int)*3,sizeof(int));


        //==============================================================================================================
        // Test both top and bottom stacks
        //==============================================================================================================

        delete(testStackAllocator);
        testStackAllocator = nullptr;

        free(preAllocatedMemoryPtr);
        preAllocatedMemoryPtr = nullptr;
    }

} // namespace ae