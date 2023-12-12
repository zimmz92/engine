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

    private:


    protected:

        /// The pointer to the starting memory position of the block of memory this allocator manages.
        void* m_allocatedMemoryPtr;

        /// The size of the block of memory this allocator manages.
        std::size_t m_allocatedMemorySize;

        /// Gets the next available aligned address to the desired byte alignment.
        /// \param t_addressToAlign The address for which the next byte aligned address shall be calculated starting at.
        /// \param t_byteAlignment The number of bytes that the address should be aligned to. This is defined by default
        /// at compilation and should not need to be modified but can be specified just in case.
        static void* getAlignedAddress(void* t_addressToAlign, std::size_t t_byteAlignment = MEMORY_ALIGNMENT);

        /// Gets the required offset to align the address with the desired/specified alignment.
        /// \param t_addressToAlign The address for which the next byte aligned address shall be calculated starting at.
        /// \param t_byteAlignment The number of bytes that the address should be aligned to. This is defined by default
        /// at compilation and should not need to be modified but can be specified just in case.
        static std::size_t getAlignmentOffset(void* t_addressToAlign, std::size_t t_byteAlignment = MEMORY_ALIGNMENT);
    };
}; // namespace ae_memory