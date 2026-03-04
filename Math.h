#pragma once

#include <math.h>

namespace Seltrad {
	namespace Math {

		extern "C" float sqrtf(float);
		extern "C" float sinf(float);
		extern "C" float asinf(float);
		extern "C" float cosf(float);
		extern "C" float acosf(float);
		extern "C" float fmaxf(float, float);
		extern "C" float fminf(float, float);
		using ::fabsf;
		//extern "C" inline float __cdecl fabsf(float);
		extern "C" float tanf(float);
		extern "C" float exp2f(float);
		extern "C" float powf(float, float);
		extern "C"

		inline float cotf(float _Angle) {
			return 1.f / tanf(_Angle);
		}


		constexpr float Pi = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342;
		constexpr float PiDIV2 = Pi * 0.5l;
		constexpr float PiDIV3 = Pi / 3;
		constexpr float PiDIV4 = Pi * 0.25l;
	}
}