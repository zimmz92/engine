/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies

// libraries

//std
#include <stdlib.h>
#include <new>
#include <memory>

namespace ae_memory {
    class AeAllocatorBase {
    public:
        /// Constructor of AeAllocatorBase. Currently Default.
        AeAllocatorBase(std::size_t t_allocatedMemorySize, void* t_allocatedMemoryPtr) noexcept :
                m_allocatedMemoryPtr{t_allocatedMemoryPtr},
                m_allocatedMemorySize{t_allocatedMemorySize}{};

        /// Allocate memory for the array.
        /// \param t_allocationSize The size of the memory in bytes to be allocated.
        virtual void* allocate(std::size_t t_allocationSize)=0;

        /// Deallocates the memory for an array allocated memory by this allocator.
        /// \param t_allocatedMemoryPtr The pointer to the allocated memory which is to be freed.
        virtual void deallocate(void* t_allocatedMemoryPtr)=0;

    private:

    protected:
        void* m_allocatedMemoryPtr;
        std::size_t m_allocatedMemorySize;
    };
} // namespace ae_memory