/// \file ae_memory_manager.cpp
/// The AeMemoryManager class is implemented.
#include "ae_memory_manager.hpp"

// dependencies

// libraries

// std
#include <memory>


namespace ae_memory {

    AeMemoryManager::AeMemoryManager(std::size_t t_memoryBlockSize) : m_memoryBlockSize{t_memoryBlockSize} {

        // Attempt to allocate a block of memory that to be managed by this class.
        m_allocatedMemoryPtr = malloc(m_memoryBlockSize);

        // Throw and error if the memory could not be properly allocated.
        if (!m_allocatedMemoryPtr) {
            throw std::bad_alloc();
        };
    }

    AeMemoryManager::~AeMemoryManager() {
        // Ensure the memory block is freed.
        free(m_allocatedMemoryPtr);
    }

} //namespace ae