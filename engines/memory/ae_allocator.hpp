/// \file ae_allocator.hpp
/// The AeAllocator class is defined.
#pragma once

// dependencies

// libraries

//std
#include <stdlib.h>
#include <new>
#include <memory>

namespace ae_memory {

    template <class T>
    class AeStackAllocator {
    public:
        /// Constructor of AeAllocator. Currently Default.
        AeStackAllocator() noexcept {};

        /// Copy Constructor. Currently Default.
        template <class U> AeStackAllocator(AeStackAllocator<U> const&) noexcept {};

        /// Allocate memory for the array.
        /// \param numElements The number of array elements being created.
        T* allocate(std::size_t numElements){
            // Check that the array to be created is not 0 elements in length.
            if (numElements == 0)
            {
                return nullptr;
            };

            // Check that the number of elements is not larger than the maximum allowed array length.
            if (numElements > static_cast<size_t>(-1) / sizeof(T))
            {
                throw std::bad_array_new_length();
            };

            // Attempt to allocate the memory from the heap.
            void* const array_pointer = malloc(numElements * sizeof(T));

            // If memory for some reason could not be allocated from the heap error out.
            if (!array_pointer) {
                throw std::bad_alloc();
            };

            // If memory allocation was successful return the array pointer.
            return static_cast<T*>(array_pointer);
        };

        /// Deallocates the memory for an array allocated memory by this allocator.
        /// \param array_pointer The pointer to the array for which memory is being freed.
        /// \param numElements The number of elements in the array.
        void deallocate(T* array_pointer, std::size_t numElements) noexcept
        {
            // Release the memory allocated to the array.
            free(array_pointer);
        };

        /// Implements the equals comparison operator.
        template<class U> bool operator==(const AeStackAllocator<U>&) const noexcept {
            return true;
        };

        /// Implements the not equals comparison operator.
        template<class U> bool operator!=(const AeStackAllocator<U>&) const noexcept {
            return false;
        };
    };
} // namespace ae_memory