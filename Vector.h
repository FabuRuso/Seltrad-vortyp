#pragma once
#include "Math.h"

namespace Seltrad {
	namespace Math {
		enum Coordinates {
			X = 0,
			Y = 1,
			Z = 2,
			W = 3
		};

		struct Vector {
			union {
				struct {
					float
						Xc,
						Yc,
						Zc;
				};
				float Coords[3];
			};

			constexpr Vector(float _X = 0.f, float _Y = 0.f, float _Z = 0.f) :
				Xc{ _X },
				Yc{ _Y },
				Zc{ _Z }
			{

			}

			constexpr float SQUARED_LENGTH_(void) const {
				return {
					Xc * Xc +
					Yc * Yc +
					Zc * Zc
				};
			}

			float length(void) const {
				return sqrtf(SQUARED_LENGTH_());
			}

			Vector normalize(void) const {
				float l = SQUARED_LENGTH_();
				if (l == 0.f)
					return { 0.f, 0.f, 0.f };
				l = 1.f / sqrtf(l);
				return {
					Xc * l,
					Yc * l,
					Zc * l
				};
			}

			Vector operator*(float _V) const {
				return {
					Xc * _V,
					Yc * _V,
					Zc * _V
				};
			}
			friend Vector operator*(float _Left, const Vector& _Right) {
				return _Right * _Left;
			}

			Vector operator/(float _V) const {
				return *this * (1.f / _V);
			}

			Vector operator+(const Vector& _Right) const {
				return {
					Xc + _Right.Xc,
					Yc + _Right.Yc,
					Zc + _Right.Zc
				};
			}

			Vector operator-(const Vector& _Right) const {
				return {
					Xc - _Right.Xc,
					Yc - _Right.Yc,
					Zc - _Right.Zc
				};
			}

			Vector operator-(void) const {
				return {
					-Xc,
					-Yc,
					-Zc
				};
			}

			float operator[](Coordinates _C) const {
				return Coords[_C];
			}
			float& operator[](Coordinates _C) {
				return Coords[_C];
			}

			bool constexpr operator==(const Vector& _Other) const {
				return Xc == _Other.Xc and Yc == _Other.Yc and Zc == _Other.Zc;
			}
			bool constexpr operator!=(const Vector& _Other) const {
				return Xc != _Other.Xc or Yc != _Other.Yc or Zc != _Other.Zc;
			}

			static float dot(const Vector& _Left, const Vector& _Right) {
				return
					_Left.Xc * _Right.Xc +
					_Left.Yc * _Right.Yc +
					_Left.Zc * _Right.Zc;
			}
			
			static Vector cross(const Vector& _Left, const Vector& _Right) {
				return {
					_Left.Yc * _Right.Zc - _Left.Zc * _Right.Yc,
					_Left.Zc * _Right.Xc - _Left.Xc * _Right.Zc,
					_Left.Xc * _Right.Yc - _Left.Yc * _Right.Xc,
				};
			}

			static Vector hadamard(const Vector& _Left, const Vector& _Right) {
				return {
					_Left.Xc * _Right.Xc,
					_Left.Yc * _Right.Yc,
					_Left.Zc * _Right.Zc,
				};
			}

			static float cos(const Vector& _Left, const Vector& _Right) {
				float L = _Left.SQUARED_LENGTH_() * _Right.SQUARED_LENGTH_();
				if (L == 0)
					return 0;
				return dot(_Left, _Right) / sqrtf(L);
			}

			static float angle(const Vector& _Left, const Vector& _Right) {
				return acosf(cos(_Left, _Right));
			}

			static constexpr Vector X_plus() {
				return { 1.f, 0.f, 0.f };
			}
			static constexpr Vector X_minus() {
				return { -1.f, 0.f, 0.f };
			}

			static constexpr Vector Y_plus() {
				return { 0.f, 1.f, 0.f };
			}
			static constexpr Vector Y_minus() {
				return { 0.f, -1.f, 0.f };
			}

			static constexpr Vector Z_plus() {
				return { 0.f, 0.f, 1.f };
			}
			static constexpr Vector Z_minus() {
				return { 0.f, 0.f, -1.f };
			}
			static constexpr Vector Zero() {
				return { 0.f, 0.f, 0.f };
			}
			static constexpr Vector Unit() {
				return { 1.f, 1.f, 1.f };
			}
		};

		class SmartVector {
			union {
				struct {
					float
						X__,
						Y__,
						Z__;
				};
				float Coords__[3];
			};
			mutable float Length__ = -1;
			mutable Vector Norm = { 1,1,1 };
		public:
			SmartVector(float _X, float _Y, float _Z) :
				X__{ _X },
				Y__{ _Y },
				Z__{ _Z }
			{

			}

			float length(void) const {
				if (Length__ == -1.f)
					Length__ = sqrtf(
						X__ * X__ +
						Y__ * Y__ +
						Z__ * Z__
					);
				return Length__;
			}

			Vector normalize(void) const {
				if (length() == 0.f)
					Norm = { 0.f, 0.f, 0.f };
				else
					if (Norm == Vector::Unit()) {
						float L = 1.f / Length__;
						Norm = {
							X__ * L,
							Y__ * L,
							Z__ * L
						};
					}
				return Norm;
			}

			SmartVector(const Vector& _V) : SmartVector(_V.Xc, _V.Yc, _V.Zc) {

			}

			SmartVector(const SmartVector& _V) = default;

			float operator[](Coordinates _C) const {
				return Coords__[_C];
			}

			SmartVector operator*(float _V) const {
				return {
					X__ * _V,
					Y__ * _V,
					Z__ * _V
				};
			}
			friend SmartVector operator*(float _Left, const SmartVector& _Right) {
				return _Right * _Left;
			}

			SmartVector operator/(float _V) const {
				return *this * (1.f / _V);
			}

			SmartVector operator+(const SmartVector& _Other) const {
				return {
					X__ + _Other.X__,
					Y__ + _Other.Y__,
					Z__ + _Other.Z__
				};
			}

			SmartVector operator-(const SmartVector& _Other) const {
				return {
					X__ - _Other.X__,
					Y__ - _Other.Y__,
					Z__ - _Other.Z__
				};
			}

			SmartVector operator-(void) const {
				return {
					-X__,
					-Y__,
					-Z__
				};
			}

			static float dot(const SmartVector& _Left, const SmartVector& _Right) {
				return
					_Left.X__ * _Right.X__ +
					_Left.Y__ * _Right.Y__ +
					_Left.Z__ * _Right.Z__;
			}

			static Vector cross(const SmartVector& _Left, const SmartVector& _Right) {
				return {
					_Left.Y__ * _Right.Z__ - _Left.Z__ * _Right.Y__,
					_Left.Z__ * _Right.X__ - _Left.X__ * _Right.Z__,
					_Left.X__ * _Right.Y__ - _Left.Y__ * _Right.X__,
				};
			}

			static Vector hadamard(const SmartVector& _Left, const SmartVector& _Right) {
				return {
					_Left.X__ * _Right.X__,
					_Left.Y__ * _Right.Y__,
					_Left.Z__ * _Right.Z__,
				};
			}

			static float cos(const SmartVector& _Left, const SmartVector& _Right) {
				float L = _Left.length() * _Right.length();
				if (L == 0)
					return 0;
				return dot(_Left, _Right) / L;
			}

			static float angle(const SmartVector& _Left, const SmartVector& _Right) {
				return acosf(cos(_Left, _Right));
			}
		};

	}
}