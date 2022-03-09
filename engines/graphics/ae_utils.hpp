#pragma once

#include <functional>

namespace ae {

	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& t_seed, const T& t_v, const Rest&... t_rest) {
		t_seed ^= std::hash<T>{}(t_v)+0x9e3779b9 + (t_seed << 6) + (t_seed >> 2);
		(hashCombine(t_seed, t_rest), ...);
	};

}  // namespace ae