#pragma once

#include <cstdint>

namespace ae {
	class AeIdCounters {

		static inline std::int64_t entityTypeIdCount = 0;
		static inline std::int64_t componentIdCount = 0;

	public:

		template <class T> 
		static const std::int64_t allocateEntityTypeId() {
			static const std::int64_t staticTypeId{ entityTypeIdCount++ };
			return staticTypeId;
		};

		template <class T>
		static const std::int64_t allocateComponentId() {
			static const std::int64_t staticTypeId{ componentIdCount++ };
			return staticTypeId;
		};
	};
};