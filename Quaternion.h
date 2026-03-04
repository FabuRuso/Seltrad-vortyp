#pragma once
#include "Vector.h"

namespace Seltrad {
	namespace Math {

		struct Quaternion {
			union {
				struct {
					union {
						struct {
							float X, Y, Z;
						};
						struct {
							Vector N;
						};
					};
					float W;
				};
				float Coords[4];
			};

			Quaternion(float _X, float _Y, float _Z, float _W) :
				X{ _X },
				Y{ _Y },
				Z{ _Z },
				W{ _W }
			{
				this->normalize();
			}

			Quaternion(const Vector& _Other) : N{ _Other }, W{ 0.f } {

			}

			Quaternion(const Vector& _Axis, float _Rads) : N{ _Axis.normalize() * sinf(_Rads / 2.f) }, W{ cosf(_Rads / 2.f) } {

			}

			Quaternion(float _UmX, float _UmY, float _UmZ) {
				*this =
					Quaternion({ 1.f, 0.f, 0.f }, _UmX)*
					Quaternion({ 0.f, 1.f, 0.f }, _UmY)*
					Quaternion({ 0.f, 0.f, 1.f }, _UmZ);
			}

			Quaternion& normalize(void) {
				float L = X * X + Y * Y + Z * Z + W * W;
				if (L == 0)
					return *this;
				L = 1.f / sqrtf(L);
				X *= L;
				Y *= L;
				Z *= L;
				W *= L;
				return *this;
			}

			Quaternion conjugate(void) const {
				return {
					-X,
					-Y,
					-Z,
					W
				};
			}

			float& operator[](Coordinates _C) {
				return Coords[_C];
			}

			Quaternion operator*(const Quaternion& _Other) const {
				float scalar = W * _Other.W - Vector::dot(N, _Other.N);
				Vector normal = W * _Other.N + _Other.W * N + Vector::cross(N, _Other.N);
				Quaternion Res{};
				Res.N = normal;
				Res.W = scalar;
				return Res;
			}

		private:
			Quaternion(void) :X{ 0 }, Y{ 0 }, Z{ 0 }, W{ 0 } {

			}
		};
	}
}