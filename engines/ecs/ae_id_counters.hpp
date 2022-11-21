#pragma once

#include <cstdint>

namespace ae {
	class AeIdCounters {

		static inline std::int64_t entityTypeIdCount = -1;

	public:

		template <class T> 
		static const std::int64_t allocateEntityTypeId() {
			static const std::int64_t staticTypeId{ entityTypeIdCount++ };
			return staticTypeId;
		};
	};
};