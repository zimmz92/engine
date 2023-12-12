/// \file ae_allocator_base.hpp
/// The AeAllocatorBase class is defined.
#pragma once

// dependencies
#include "ae_memory_constants.hpp"

// libraries

//std
#include <cstdlib>
#include <new>
#include <memory>

namespace ae_memory {

    class AeAllocatorBase {
    public:

        /// Constructor of AeAllocatorBase is given a block of pre-allocated memory to allocated as required for the
        /// game.
        /// \param t_allocatedMemorySize The size of the pre-allocated memory this allocator will be responsible for.
        /// \param t_allocatedMemoryPtr A pointer to the pre-allocated memory this allocator will be responsible for
        /// managing.
        AeAllocatorBase(std::size_t t_allocatedMemorySize, void* t_allocatedMemoryPtr) noexcept;

        /// Destructor of the AeAllocatorBase. Will free the pointer to the memory it was managing.
        ~AeAllocatorBase();

        /// Allocate memory for the array.
        /// \param t_allocationSize The size of the memory in bytes to be allocated.
        virtual void* allocate(std::size_t t_allocationSize)=0;

        /// Deallocates the memory for an array allocated memory by this allocator.
        /// \param t_allocatedMemoryPtr The pointer to the allocated memory which is to be freed.
        virtual void deallocate(void* t_allocatedMemoryPtr)=0;

        /// Gets the size of the pre-allocated memory block that was given to this allocator to manage.
        [[nodiscard]] std::size_t getAllocatedMemorySize() const;

        /// Gets the pointer to the pre-allocated memory block that was given to this allocator to manage.
        void* getAllocatedMemoryPtr();

        /// Gets the next available aligned address to the desired byte alignment.
        /// \param t_addressToAlign The address for which the next byte aligned address shall be calculated starting at.
        /// \param t_byteAlignment The number of bytes that the address should be aligned to. This MUST be a power of 2.
        /// This is defined by default at compilation and should not need to be modified but can be specified just in
        /// case. Typically used for linear allocators.
        static void* getAlignedAddress(void* t_addressToAlign, std::size_t t_byteAlignment = MEMORY_ALIGNMENT);

        /// Gets the required offset to align the address with the desired/specified alignment.
        /// \param t_addressToAlign The address for which the next byte aligned address shall be calculated starting at.
        /// \param t_byteAlignment The number of bytes that the address should be aligned to. This MUST be a power of 2!
        /// This is defined by default at compilation and should not need to be modified but can be specified just in
        /// case.
        static std::size_t getAlignmentOffset(void* t_addressToAlign, std::size_t t_byteAlignment = MEMORY_ALIGNMENT);

        /// Gets the next available aligned address. The alignment offset required to align the address will be stored
        /// in the byte preceding the returned aligned address. If the provided address is already aligned then the
        /// returned aligned address will be incremented by the full alignment size to make room for storing the
        /// alignment offset. This function only works up to a alignment of 256. Typically used for stack allocators.
        /// \param t_addressToAlign The address for which the next byte aligned address shall be calculated starting at.
        /// \param t_byteAlignment The number of bytes that the address should be aligned to. This MUST be a power of 2!
        /// This is defined by default at compilation and should not need to be modified but can be specified just in
        /// case.
        static void* getAlignedAddressWithStoredOffset(void* t_addressToAlign, std::size_t t_byteAlignment = MEMORY_ALIGNMENT);

        /// Gets the next available aligned address which allows for the minimum offset. The user is responsible for
        /// keeping track of the provided minimum offset as only the additional alignment offset required to
        /// align the address will be stored in the byte preceding the returned aligned address. If the provided address
        /// is already aligned then the returned aligned address will be incremented by the full alignment size to make
        /// room for storing the alignment offset. This function only works up to a alignment of 256.
        /// \param t_addressToAlign The address for which the next byte aligned address, with at least the minimum
        /// offset + offset size storage, shall be calculated starting at.
        /// \param t_byteAlignment The number of bytes that the address should be aligned to. This MUST be a power of 2!
        /// This is defined by default at compilation and should not need to be modified but can be specified just in
        /// case.
        /// \param t_minimumOffset The minimum amount of offset, in bytes, that is required to be available preceding
        /// the returned address.
        static void* getAlignedAddressWithMinimumOffset(void* t_addressToAlign,
                                                        std::size_t t_minimumOffset,
                                                        std::size_t t_byteAlignment = MEMORY_ALIGNMENT);

        /// Gets the next available aligned address which allows for the specified number of void pointer offsets to
        /// precede the aligned address. The user is responsible for keeping track of how many void pointer offsets were
        /// requested since the assumption is that this function is primarily used to ensure there is enough memory for
        /// a void pointer(s) to be available preceding the returned pointer that will point to the previous and/or next
        /// memory location(s) that were allocated from this allocator. Typically used for freelist allocators.
        /// \param t_addressToAlign The address for which the next byte aligned address, with enough offset to store the
        ///  desired number of void pointers, shall be calculated starting at.
        /// \param t_byteAlignment The number of bytes that the address should be aligned to. This MUST be a power of 2!
        /// This is defined by default at compilation and should not need to be modified but can be specified just in
        /// case.
        /// \param t_numVoidPtrOffsets The minimum amount of void pointers that are required to be available preceding
        /// the returned address.
        static void* getAlignedAddressWithVoidPtrOffsets(void* t_addressToAlign,
                                                         std::size_t t_numVoidPtrOffsets,
                                                         std::size_t t_byteAlignment = MEMORY_ALIGNMENT);

        /// Returns the base address that was then aligned.
        /// \param t_alignedAddress The aligned address that the base address must be calculated from.
        /// \param t_minimumOffsetUsed Defaults to 0. If a minimum offset was supplied when the address was originally
        /// aligned it must be provided here to correctly calculate the base address.
        static void* getBaseAddressFromAlignedAddress(void* t_alignedAddress, std::size_t t_minimumOffsetUsed = 0);

    private:


    protected:

        /// The pointer to the starting memory position of the block of memory this allocator manages.
        void* m_allocatedMemoryPtr;

        /// The size of the block of memory this allocator manages.
        std::size_t m_allocatedMemorySize;

    };
}; // namespace ae_memory