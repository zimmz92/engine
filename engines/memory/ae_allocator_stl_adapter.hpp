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

        typedef T value_type;


        AeAllocatorStlAdaptor() = delete;

        AeAllocatorStlAdaptor(Alloc& allocator) noexcept : m_allocator(allocator){

        };

        template<typename U>
        AeAllocatorStlAdaptor(const AeAllocatorStlAdaptor<U, Alloc>& other) noexcept : m_allocator(other.m_allocator){

        };

        [[nodiscard]] constexpr T* allocate(std::size_t n)
        {
            return reinterpret_cast<T*>
            (m_allocator.Allocate(n * sizeof(T), alignof(T)));
        }

        constexpr void deallocate(T* p, [[maybe_unused]] std::size_t n)
        noexcept
        {
            m_allocator.Free(p);
        }

        std::size_t MaxAllocationSize() const noexcept
        {
            return m_allocator.GetSize();
        }

        bool operator==(const AeAllocatorStlAdaptor<T,Alloc>& rhs) const noexcept
        {

        }

        bool operator!=(const AeAllocatorStlAdaptor<T,Alloc>& rhs) const noexcept
        {
            return !(*this == rhs);
        }


        Alloc& m_allocator;
    };
} // namespace ae_memory