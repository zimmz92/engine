/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_allocator_base.hpp"

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
        std::size_t storedAlignmentOffset = static_cast<std::size_t>(reinterpret_cast<std::uint8_t*>(alignedAddressWStoredOffset)[-1]);
        void* baseAddressFromStoredOffset = ae_memory::AeAllocatorBase::getBaseAddressFromAlignedAddress(alignedAddressWStoredOffset);

        void* alignedAddressWMinimumOffset = ae_memory::AeAllocatorBase::getAlignedAddressWithMinimumOffset(starting_ptr,sizeof(std::uint8_t)*3);
        std::size_t storedAlignmentOffsetMinimumOffset = static_cast<std::size_t>(reinterpret_cast<std::uint8_t*>(alignedAddressWMinimumOffset)[-1]);
        void* baseAddressFromMinOffset = ae_memory::AeAllocatorBase::getBaseAddressFromAlignedAddress(alignedAddressWMinimumOffset,sizeof(std::uint8_t)*3);

        void* alignedAddressWVoidOffset = ae_memory::AeAllocatorBase::getAlignedAddressWithVoidPtrOffsets(starting_ptr,2);
        std::size_t storedAlignmentOffsetVoidOffset = static_cast<std::size_t>(reinterpret_cast<std::uint8_t*>(alignedAddressWVoidOffset)[-1]);
        void* baseAddressFromVoid = ae_memory::AeAllocatorBase::getBaseAddressFromAlignedAddress(alignedAddressWVoidOffset,sizeof(void*)*2);

        free(base_ptr);

    };

} // namespace ae