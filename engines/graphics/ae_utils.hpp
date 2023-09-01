#pragma once

#include <functional>

namespace ae {

    // A general hashing function that will only compile for the number of types T and rest types +1
	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& t_seed, const T& t_v, const Rest&... t_rest) {
        // Calculate a portion of the hash value.
		t_seed ^= std::hash<T>{}(t_v)+0x9e3779b9 + (t_seed << 6) + (t_seed >> 2);
        // Use Folding to call the hashCombine function on each of the t_rest inputs.
		(hashCombine(t_seed, t_rest), ...);
	};

}  // namespace ae