#pragma once

#include <cstdint>

namespace ae {

	class AeComponent {
	public:
		AeComponent();
		~AeComponent();

	private:

	protected:
		std::uint64_t m_componentId;
	};


}