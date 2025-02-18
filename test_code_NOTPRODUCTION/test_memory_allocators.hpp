/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_allocator_base.hpp"
#include "ae_stack_allocator.hpp"
#include "ae_de_stack_allocator.hpp"
#include "ae_allocator_stl_adapter.hpp"
#include "ae_pool_allocator.hpp"
#include "ae_free_linked_list_allocator.hpp"
#include "stl_wrappers.hpp"

// libraries

// std
#include <vector>
#include <cmath>

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

        auto* testDeStackAllocator = new ae_memory::AeDeStackAllocator(preAllocatedSize, preAllocatedMemoryPtr);

        //==============================================================================================================
        // Test the bottom stack
        //==============================================================================================================

        // Test that the stack allocator properly gives out memory and the proper stack markers.
        int* testIntegerArrayA = (int*)testDeStackAllocator->allocateFromBottom(sizeof(int) * 5, sizeof(int));
        ae_memory::AeDeStackAllocator::BottomStackMarker postAMarker = testDeStackAllocator->getBottomStackMarker();
        int* testIntegerArrayB = (int*)testDeStackAllocator->allocateFromBottom(sizeof(int) * 6, sizeof(int));
        ae_memory::AeDeStackAllocator::BottomStackMarker postBMarker = testDeStackAllocator->getBottomStackMarker();
        int* testIntegerArrayC = (int*)testDeStackAllocator->allocateFromBottom(sizeof(int) * 3, sizeof(int));

        // Ensure deallocation to markers works properly.
        testDeStackAllocator->deallocateToBottomMarker(postBMarker);
        testDeStackAllocator->deallocateToBottomMarker(postAMarker);

        // Check to see if clearing the stack works.
        testIntegerArrayC = (int*)testDeStackAllocator->allocateFromBottom(sizeof(int) * 7, sizeof(int));
        testDeStackAllocator->clearBottomStack();

        // Ensure that the stack will throw an error as expected if too much memory is being allocated from it.
        //testIntegerArrayA = (int*)testStackAllocator->allocateFromBottom(sizeof(int)*30,sizeof(int));
        //testIntegerArrayB = (int*)testStackAllocator->allocateFromBottom(sizeof(int)*2,sizeof(int));
        //testIntegerArrayC = (int*)testStackAllocator->allocateFromBottom(sizeof(int)*3,sizeof(int));

        //==============================================================================================================
        // Test the top stack
        //==============================================================================================================
        // Test that the stack allocator properly gives out memory and the proper stack markers.
        testIntegerArrayA = (int*)testDeStackAllocator->allocateFromTop(sizeof(int) * 5, sizeof(int));
        ae_memory::AeDeStackAllocator::TopStackMarker postATopMarker = testDeStackAllocator->getTopStackMarker();
        testIntegerArrayB = (int*)testDeStackAllocator->allocateFromTop(sizeof(int) * 6, sizeof(int));
        ae_memory::AeDeStackAllocator::TopStackMarker postBTopMarker = testDeStackAllocator->getTopStackMarker();
        testIntegerArrayC = (int*)testDeStackAllocator->allocateFromTop(sizeof(int) * 3, sizeof(int));

        // Ensure deallocation to markers works properly.
        testDeStackAllocator->deallocateToTopMarker(postBTopMarker);
        testDeStackAllocator->deallocateToTopMarker(postATopMarker);

        // Check to see if clearing the stack works.
        testIntegerArrayC = (int*)testDeStackAllocator->allocateFromTop(sizeof(int) * 7, sizeof(int));
        testDeStackAllocator->clearTopStack();

        // Ensure that the stack will throw an error as expected if too much memory is being allocated from it.
        //testIntegerArrayA = (int*)testStackAllocator->allocateFromTop(sizeof(int)*30,sizeof(int));
        //testIntegerArrayB = (int*)testStackAllocator->allocateFromTop(sizeof(int)*2,sizeof(int));
        //testIntegerArrayC = (int*)testStackAllocator->allocateFromTop(sizeof(int)*3,sizeof(int));


        //==============================================================================================================
        // Test both top and bottom stacks
        //==============================================================================================================

        // Test that the stack allocator properly gives out memory and the proper stack markers.
        testIntegerArrayA = (int*)testDeStackAllocator->allocateFromTop(sizeof(int) * 5, sizeof(int));
        ae_memory::AeDeStackAllocator::TopStackMarker postADeMarker = testDeStackAllocator->getTopStackMarker();

        testIntegerArrayB = (int*)testDeStackAllocator->allocateFromBottom(sizeof(int) * 5, sizeof(int));
        ae_memory::AeDeStackAllocator::BottomStackMarker postBDeMarker = testDeStackAllocator->getBottomStackMarker();

        testIntegerArrayC = (int*)testDeStackAllocator->allocateFromTop(sizeof(int) * 3, sizeof(int));
        ae_memory::AeDeStackAllocator::TopStackMarker postCDeMarker = testDeStackAllocator->getTopStackMarker();

        int* testIntegerArrayD = (int*)testDeStackAllocator->allocateFromBottom(sizeof(int) * 5, sizeof(int));
        ae_memory::AeDeStackAllocator::BottomStackMarker postDDeMarker = testDeStackAllocator->getBottomStackMarker();

        int* testIntegerArrayE = (int*)testDeStackAllocator->allocateFromTop(sizeof(int) * 3, sizeof(int));
        ae_memory::AeDeStackAllocator::TopStackMarker postEDeMarker = testDeStackAllocator->getTopStackMarker();

        int* testIntegerArrayF = (int*)testDeStackAllocator->allocateFromBottom(sizeof(int) * 5, sizeof(int));

        // Ensure deallocation to markers works properly.
        testDeStackAllocator->deallocateToTopMarker(postEDeMarker);
        testDeStackAllocator->deallocateToBottomMarker(postDDeMarker);
        testDeStackAllocator->deallocateToTopMarker(postCDeMarker);
        testDeStackAllocator->deallocateToBottomMarker(postBDeMarker);
        testDeStackAllocator->deallocateToTopMarker(postADeMarker);


        // Check to see if clearing the stack works.
        testIntegerArrayC = (int*)testDeStackAllocator->allocateFromTop(sizeof(int) * 7, sizeof(int));
        testIntegerArrayF = (int*)testDeStackAllocator->allocateFromBottom(sizeof(int) * 5, sizeof(int));
        testDeStackAllocator->clearDoubleEndedStack();

        // Ensure that the stack will throw an error as expected if too much memory is being allocated from it.
        testIntegerArrayA = (int*)testDeStackAllocator->allocateFromTop(sizeof(int) * 15, sizeof(int));
        testIntegerArrayB = (int*)testDeStackAllocator->allocateFromBottom(sizeof(int) * 15, sizeof(int));
        //testIntegerArrayC = (int*)testStackAllocator->allocateFromTop(sizeof(int)*3,sizeof(int));
        testIntegerArrayE = (int*)testDeStackAllocator->allocateFromBottom(sizeof(int) * 3, sizeof(int));


        // Clear the pointers
        delete(testDeStackAllocator);
        testDeStackAllocator = nullptr;

        free(preAllocatedMemoryPtr);
        preAllocatedMemoryPtr = nullptr;
    }

    void test_stl_adapter(){
        // In bytes.
        std::size_t preAllocatedSize = 128;
        void* preAllocatedMemoryPtr = malloc(preAllocatedSize);

        ae_memory::AeStackAllocator testStackAllocator = ae_memory::AeStackAllocator(preAllocatedSize,preAllocatedMemoryPtr);
        std::vector<int, ae_memory::AeAllocatorStlAdaptor<int,ae_memory::AeStackAllocator>> myVector(testStackAllocator);

        myVector.push_back(5);

        std::vector<int, ae_memory::AeAllocatorStlAdaptor<int,ae_memory::AeStackAllocator>> myVectorTwo(testStackAllocator);

        myVectorTwo.push_back(3);
        myVector.push_back(10);


        testStackAllocator.clearStack();

        struct TestStruct {
            std::float_t rho = 0.0f;
            std::float_t theta = 0.0f;
            std::float_t phi = 0.0f;
        };

        unordered_map<int,TestStruct,ae_memory::AeAllocatorBase> myUnorderedMap(testStackAllocator);

        myUnorderedMap.reserve(10);
        myUnorderedMap[1] = TestStruct();

        free(preAllocatedMemoryPtr);
        preAllocatedMemoryPtr = nullptr;
    };

    void test_pool_allocator(){
        // In bytes.
        std::size_t preAllocatedSize = 32;
        void* preAllocatedMemoryPtr = malloc(preAllocatedSize);

        // Create a test structure to allocate pools from.
        struct testStruct{
            int x = 5;
            float y = 5.0f;
        };

        // Create a pool allocator to test.
        ae_memory::AePoolAllocator testPoolAllocator = ae_memory::AePoolAllocator(preAllocatedSize,
                                                                                  preAllocatedMemoryPtr,
                                                                                  sizeof(testStruct));

        void* test_allocationA = testPoolAllocator.allocate(sizeof(testStruct),
                                                            ae_memory::MEMORY_ALIGNMENT);
        void* test_allocationB = testPoolAllocator.allocate(sizeof(testStruct),
                                                            ae_memory::MEMORY_ALIGNMENT);
        void* test_allocationC = testPoolAllocator.allocate(sizeof(testStruct),
                                                            ae_memory::MEMORY_ALIGNMENT);
        void* test_allocationD = testPoolAllocator.allocate(sizeof(testStruct),
                                                            ae_memory::MEMORY_ALIGNMENT);

        testPoolAllocator.deallocate(test_allocationB);
        testPoolAllocator.deallocate(test_allocationC);

        void* test_allocationE = testPoolAllocator.allocate(sizeof(testStruct),
                                                            ae_memory::MEMORY_ALIGNMENT);

        void* test_allocationF = testPoolAllocator.allocate(sizeof(testStruct),
                                                            ae_memory::MEMORY_ALIGNMENT);

        // This should fail so comment out after testing this due to the lack of memory available.
//        void* test_allocationG = testPoolAllocator.allocate(sizeof(testStruct),
//                                                            ae_memory::MEMORY_ALIGNMENT);


        // Clear the memory
        testPoolAllocator.deallocate(test_allocationA);
        testPoolAllocator.deallocate(test_allocationD);
        testPoolAllocator.deallocate(test_allocationE);
        testPoolAllocator.deallocate(test_allocationF);

        // Free the memory allocated for the allocator.
        free(preAllocatedMemoryPtr);
        preAllocatedMemoryPtr = nullptr;
    };

    void test_free_linked_list_allocator(){
        // In bytes.
        std::size_t preAllocatedSize = 128;
        void* preAllocatedMemoryPtr = malloc(preAllocatedSize);

        // Create a test structure to allocate pools from.
        struct testStruct{
            int x = 5;
            float y = 5.0f;
        };

        // Create a pool allocator to test.
        ae_memory::AeFreeLinkedListAllocator testFreeLinkedListAllocator = ae_memory::AeFreeLinkedListAllocator(preAllocatedSize,
                                                                                                                preAllocatedMemoryPtr);

        void* test_allocationA = testFreeLinkedListAllocator.allocate(sizeof(testStruct),
                                                                      ae_memory::MEMORY_ALIGNMENT);
        void* test_allocationB = testFreeLinkedListAllocator.allocate(sizeof(testStruct),
                                                                      ae_memory::MEMORY_ALIGNMENT);
        void* test_allocationC = testFreeLinkedListAllocator.allocate(sizeof(testStruct),
                                                                      ae_memory::MEMORY_ALIGNMENT);
        void* test_allocationD = testFreeLinkedListAllocator.allocate(sizeof(testStruct),
                                                                      ae_memory::MEMORY_ALIGNMENT);

        testFreeLinkedListAllocator.deallocate(test_allocationB);
        testFreeLinkedListAllocator.deallocate(test_allocationC);

        void* test_allocationE = testFreeLinkedListAllocator.allocate(sizeof(testStruct),
                                                                      ae_memory::MEMORY_ALIGNMENT);

        void* test_allocationF = testFreeLinkedListAllocator.allocate(sizeof(testStruct),
                                                                      ae_memory::MEMORY_ALIGNMENT);

        // This should fail so comment out after testing this due to the lack of memory available.
//        void* test_allocationG = testPoolAllocator.allocate(sizeof(testStruct),
//                                                            ae_memory::MEMORY_ALIGNMENT);

        // Clear the memory
        testFreeLinkedListAllocator.deallocate(test_allocationA);
        testFreeLinkedListAllocator.deallocate(test_allocationD);
        testFreeLinkedListAllocator.deallocate(test_allocationE);
        testFreeLinkedListAllocator.deallocate(test_allocationF);


        // Clear the memory
        free(preAllocatedMemoryPtr);
        preAllocatedMemoryPtr = nullptr;
    };

} // namespace ae