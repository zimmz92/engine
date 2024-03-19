/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_allocator_stl_adapter.hpp"

// libraries

// std
#include <unordered_map>

namespace ae {
    template<typename Key, typename T, typename Alloc, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
    using unordered_map = std::unordered_map<Key, T, Hash, KeyEqual, ae_memory::AeAllocatorStlAdaptor<std::pair<const Key, T>, Alloc>>;
} // namespace ae