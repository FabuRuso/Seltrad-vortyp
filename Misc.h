#pragma once

namespace Seltrad {
	namespace Helpfulness {

		template<typename A, typename B>
		constexpr bool is_equal_t = false;

		template<typename A>
		constexpr bool is_equal_t<A, A> = true;
	}
	namespace INT {
		using  int8t = char;
		using uint8t = unsigned char;

		using  int16t = short;
		using uint16t = unsigned short;

		using  int32t = int;
		using uint32t = unsigned int;
	}
}