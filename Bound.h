#pragma once
#include "Misc.h"
#include "Vector.h"
#include "Ray.h"

namespace Seltrad {
	namespace Helpfulness {

		template<typename TYPE>
		class Bound {
			static_assert(
				is_equal_t<TYPE, int>          ||
				is_equal_t<TYPE, unsigned int> ||
				is_equal_t<TYPE, float>        ||
				is_equal_t<TYPE, Math::Vector> ||
				is_equal_t<TYPE, Engine::Ray>,
				"Not supposed type to bounding\n"
				"Bounding is available only for:\n"
				"	<float>,\n"
				"	<Math::Vector>\n"
				"	<Engine::Ray>\n");

			TYPE* Array__ = nullptr;
			size_t Length__ = 0;
			constexpr Bound(void) {}
		public:
			Bound(size_t _Length) : Length__{ _Length } {
				Array__ = new TYPE[Length__]();
			}

			Bound(const Bound& _Other) : Length__{ _Other.Length__ } {
				Array__ = new TYPE[Length__];
				for (size_t i = 0; i < Length__; i++)
					this->Array__[i] = _Other.Array__[i];
			}

			Bound(Bound&& _Other) noexcept :
				Length__{ _Other.Length__ },
				Array__{ _Other.Array__ } {
				_Other.Array__ = nullptr;
				_Other.Length__ = 0;
			}

			size_t length(void) const {
				return Length__;
			}

			TYPE& operator[](size_t _Index) {
				return Array__[_Index];
			}

			const TYPE& operator[](size_t _Index) const {
				return Array__[_Index];
			}

			Bound& operator=(const Bound& _Other) {
				if (this == &_Other)
					return *this;
				TYPE* Temp = new TYPE[_Other.Length__]();
				for (size_t i = 0; i < _Other.Length__; i++)
					Temp[i] = _Other.Array__[i];
				Length__ = _Other.Length__;
				delete[] Array__;
				Array__ = Temp;
				return *this;
			}

			TYPE* begin(void) {
				return Array__;
			}
			TYPE* end(void) {
				return Array__ + Length__;
			}

			~Bound(void) {
				delete[] Array__;
			}

			template<typename... ARGS>
			static Bound from(ARGS... args) {
				Bound Temp{ sizeof...(args) };
				TYPE Args[] = { args... };
				for (size_t i = 0; i < Temp.length(); i++)
					Temp[i] = Args[i];
				return Temp;
			}

			static constexpr Bound Empty(void) {
				return{};
			}
		};
	}
}