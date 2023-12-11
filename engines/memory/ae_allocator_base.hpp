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
        AeAllocatorBase(std::size_t t_allocatedMemorySize, void* t_allocatedMemoryPtr) noexcept :
                m_allocatedMemoryPtr{t_allocatedMemoryPtr},
                m_allocatedMemorySize{t_allocatedMemorySize}{};

        /// Destructor of the AeAllocatorBase. Will free the pointer to the memory it was managing.
        ~AeAllocatorBase(){
            m_allocatedMemoryPtr = nullptr;
        };

        /// Allocate memory for the array.
        /// \param t_allocationSize The size of the memory in bytes to be allocated.
        virtual void* allocate(std::size_t t_allocationSize)=0;

        /// Deallocates the memory for an array allocated memory by this allocator.
        /// \param t_allocatedMemoryPtr The pointer to the allocated memory which is to be freed.
        virtual void deallocate(void* t_allocatedMemoryPtr)=0;

        /// Gets the size of the pre-allocated memory block that was given to this allocator to manage.
        [[nodiscard]] std::size_t getAllocatedMemorySize() const{return m_allocatedMemorySize;};

        /// Gets the pointer to the pre-allocated memory block that was given to this allocator to manage.
        void* getAllocatedMemoryPtr(){return m_allocatedMemoryPtr;};

    private:


    protected:

        /// The pointer to the starting memory position of the block of memory this allocator manages.
        void* m_allocatedMemoryPtr;

        /// The size of the block of memory this allocator manages.
        std::size_t m_allocatedMemorySize;

        /// Gets the next available aligned address to the desired byte alignment. This by default at compilation and
        /// should not need to be modified but provided just in case.
        /// \param t_addressToAlign The address for which the next byte aligned address shall be calculated starting at.
        /// \param t_byteAlignment The number of bytes that the address should be aligned to.
        void* getAlignedAddress(void* t_addressToAlign, size_t t_byteAlignment = MEMORY_ALIGNMENT){
            // Let's break this down, first
            // 1) A reinterpret cast is done to t_addressToAlign in order to make it the same address type as the other
            // addressing being calculated.
            // 2)
            auto addressToAlign = reinterpret_cast<size_t>(t_addressToAlign);
            auto byteMask = (~(t_byteAlignment - 1));
            auto byteOffset = t_byteAlignment - 1;

            return (void*)((addressToAlign + byteOffset) & byteMask);
        }

    };
}; // namespace ae_memory