#pragma once
#include "Quaternion.h"

namespace Seltrad {
	namespace Engine {
		class Ray {
			Math::Vector
				Position,
				Direction;
		public:
			Ray(void) :
				Position{ 0.f, 0.f, 0.f },
				Direction{ 0.f, 0.f, 0.f }
			{

			}
			Ray(const Math::Vector& _Pos, const Math::Vector& _Dir) :
				Position{_Pos},
				Direction{_Dir.normalize()}
			{

			}
			Ray(const Ray& _Other) : Position{ _Other.getPos() }, Direction{ _Other.getDir() } {

			}

			Ray& operator=(const Ray& _Other) {
				Position = _Other.getPos();
				Direction = _Other.getDir();
				return *this;
			}

			Ray& rotateX(float _Rads) {
				float
					s = Math::sinf(_Rads),
					c = Math::cosf(_Rads);
				Direction = {
					Direction.Xc,
					Direction.Yc * c - Direction.Zc * s,
					Direction.Yc * s + Direction.Zc * c
				};
				return *this;
			}

			Ray& rotateY(float _Rads) {
				float
					s = Math::sinf(_Rads),
					c = Math::cosf(_Rads);
				Direction = {
					Direction.Xc * c + Direction.Zc * s,
					Direction.Yc,
					-Direction.Xc * s + Direction.Zc * c
				};
				return *this;
			}

			Ray& rotateZ(float _Rads) {
				float
					s = Math::sinf(_Rads),
					c = Math::cosf(_Rads);
				Direction = {
					Direction.Xc * c - Direction.Yc * s,
					Direction.Xc * s + Direction.Yc * c,
					Direction.Zc
				};
				return *this;
			}

			Ray& rotateAxis(const Math::Vector& _Axis, float _Rads) {
				using namespace Math;
				Quaternion Axis{ _Axis,_Rads };
				Direction = (Axis * Quaternion{ Direction } *Axis.conjugate()).N.normalize();
				return *this;
			}

			Math::Vector pointAt(float _t) const {
				return Position + _t * Direction;
			}

			Math::Vector getPos(void) const {
				return Position;
			}

			Math::Vector getDir(void) const {
				return Direction;
			}

			Ray relateTo(const Math::Vector& _Point, const Math::Quaternion& _Rotation) const {
				Math::Quaternion Conj = _Rotation.conjugate();
				Ray Result{};
				Result.Position = (Conj * (Position - _Point) * _Rotation).N;
				Result.Direction = (Conj * Direction * _Rotation).N;
				return Result;
			}
		};
	}
}