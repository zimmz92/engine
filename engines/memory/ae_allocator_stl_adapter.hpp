/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies

// libraries

// std
#include "memory"

namespace ae_memory {
    template<typename T, class Alloc>
    class AeAllocatorStlAdaptor
    {
    public:
        /// Required for the stl_adapter type.
        typedef T value_type;

        /// Remove the default constructor.
        AeAllocatorStlAdaptor() = delete;

        /// Create the allocator stl library adapter by specifying which allocator shall be actually used for memory
        /// allocation and deallocation.
        /// \param t_allocator The allocator which will actually be handling memory allocation and deallocation.
        AeAllocatorStlAdaptor(Alloc& t_allocator) noexcept : m_allocator(t_allocator){

        };

        /// Copy constructor for the adapter.s
        /// \param t_allocatorStlAdaptor The allocator stl adapter to be copied into a new AeAllocatorStlAdaptor.
        template<typename U>
        AeAllocatorStlAdaptor(const AeAllocatorStlAdaptor<U, Alloc>& t_allocatorStlAdaptor) noexcept : m_allocator(t_allocatorStlAdaptor.m_allocator){

        };

        /// Passthrough to the allocate function of the allocator this adapter is the stl interface for.
        /// \param t_numElements The number of elements of type T memory is to be allocated for.
        /// \return A pointer to the start of the allocated memory for number of elements of type T.
        [[nodiscard]] constexpr T* allocate(std::size_t t_numElements)
        {
            return reinterpret_cast<T*>(m_allocator.allocate(t_numElements * sizeof(T), alignof(T)));
        };

        constexpr void deallocate(T* p, [[maybe_unused]] std::size_t n)
        noexcept
        {
            m_allocator.deallocate(p);
        };

        [[nodiscard]] std::size_t max_size() const noexcept
        {
            return m_allocator.getAllocatedMemorySize();
        };

        bool operator==(const AeAllocatorStlAdaptor<T,Alloc>& rhs) const noexcept
        {
            return *this == rhs;
        };

        bool operator!=(const AeAllocatorStlAdaptor<T,Alloc>& rhs) const noexcept
        {
            return *this != rhs;
        };

        Alloc& m_allocator;
    };
} // namespace ae_memory