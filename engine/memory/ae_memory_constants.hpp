/// \file ae_memory_constants.hpp
/// \brief The script defining constants used by the memory management engine.
/// Define the constants used by the memory management engine.

#pragma once

#include <cstdint>

namespace ae_memory{

    /// Defines the memory alignment for a given architecture.
#ifdef ARCH_64BIT
    static const std::size_t MEMORY_ALIGNMENT = 8;
#else
    #ifdef ARCH_32BIT
        static const std::size_t MEMORY_ALIGNMENT = 4;
    #else
        static const std::size_t MEMORY_ALIGNMENT = 4;
    #endif
#endif
}