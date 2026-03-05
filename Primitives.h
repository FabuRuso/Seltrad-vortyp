#pragma once
#include "Ray.h"
#include "Bound.h"

namespace Seltrad {
	namespace Engine {

		class Primitive {
		protected:
			static constexpr float PRECISION__ = 0.001;
			Math::Vector Position;
			Math::Quaternion Rotation;
			float HalfSide;
		public:

			Primitive(
				const Math::Vector& _Pos,
				float _HalfSide,
				float _RotationX,
				float _RotationY,
				float _RotationZ
			) :
				Position{ _Pos },
				Rotation{ _RotationX, _RotationY, _RotationZ },
				HalfSide{ _HalfSide }
			{

			}
			Primitive(
				const Math::Vector& _Pos,
				float _HalfSide,
				const Math::Vector& _RotationXYZ
			) :
				Primitive(
					_Pos,
					_HalfSide,
					_RotationXYZ.Xc,
					_RotationXYZ.Yc,
					_RotationXYZ.Zc)
			{

			}
			
			virtual Helpfulness::Bound<Math::Vector> getCrossPoints(const Ray&) const = 0;
			virtual Math::Vector getNormAt(const Math::Vector&) const = 0;
			virtual bool CheckLocalPointVolume(const Math::Vector&) const = 0;
		};

		class Cube : public Primitive {
		public:
			using Primitive::Primitive;
			Cube(void) = delete;

			Math::Vector getNormAt(const Math::Vector& _Point) const override {
				Math::Vector LocalP;
				if (Rotation != Math::Quaternion::ZeroRotation())
					LocalP = (Rotation.conjugate() * (_Point - Position) * Rotation).N;
				else
					LocalP = _Point - Position;
				Math::Vector Signs = {
					signbit(LocalP.Xc) ? -1.f : 1.f,
					signbit(LocalP.Yc) ? -1.f : 1.f,
					signbit(LocalP.Zc) ? -1.f : 1.f };

				LocalP = {
					Math::fabsf(LocalP.Xc),
					Math::fabsf(LocalP.Yc),
					Math::fabsf(LocalP.Zc) };

				Math::Vector Norm{};
				if (LocalP.Xc >= LocalP.Yc and LocalP.Xc >= LocalP.Zc)
					Norm = { Signs.Xc,0,0 };
				else if (LocalP.Yc >= LocalP.Xc and LocalP.Yc >= LocalP.Zc)
					Norm = { 0,Signs.Yc,0 };
				else
					Norm = { 0,0,Signs.Zc };
				if (Rotation != Math::Quaternion{ 0,0,0 })
					return (Rotation * Norm * Rotation.conjugate()).N;
				else
					return Norm;
			}

			bool CheckLocalPointVolume(const Math::Vector& _Point) const override {
				Math::Vector LocalP;
				if (_Point.Xc < 0)
					LocalP.Xc = -_Point.Xc;
				else
					LocalP.Xc = _Point.Xc;
				if (_Point.Yc < 0)
					LocalP.Yc = -_Point.Yc;
				else
					LocalP.Yc = _Point.Yc;
				if (_Point.Zc < 0)
					LocalP.Zc = -_Point.Zc;
				else
					LocalP.Zc = _Point.Zc;
				if ((LocalP.Xc > LocalP.Yc) and (LocalP.Xc > LocalP.Zc))
					return LocalP.Xc <= HalfSide + PRECISION__;
				if (LocalP.Yc > LocalP.Zc)
					return LocalP.Yc <= HalfSide + PRECISION__;
				else
					return LocalP.Zc <= HalfSide + PRECISION__;
			}

			Helpfulness::Bound<Math::Vector> getCrossPoints(const Ray& _BaseRay) const override {
				Ray LocalRay;
				if (Rotation != Math::Quaternion::ZeroRotation())
					LocalRay = _BaseRay.relateTo(Position, Rotation);
				else
					LocalRay = { _BaseRay.getPos() - Position,_BaseRay.getDir() };

				float RawPoints[6];

				Math::Vector RayDir = LocalRay.getDir();
				Math::Vector RayPos = LocalRay.getPos();

				if (RayDir.Xc != 0.f)
				{
					float x = 1.f / RayDir.Xc;
					RawPoints[0] = (+HalfSide - RayPos.Xc) * x;
					RawPoints[1] = (-HalfSide - RayPos.Xc) * x;
				}
				else
				{
					RawPoints[0] = 0;
					RawPoints[1] = 0;
				}
				if (RayDir.Yc != 0.f)
				{
					float y = 1.f / RayDir.Yc;
					RawPoints[2] = (+HalfSide - RayPos.Yc) * y;
					RawPoints[3] = (-HalfSide - RayPos.Yc) * y;
				}
				else
				{
					RawPoints[2] = 0;
					RawPoints[3] = 0;
				}
				if (RayDir.Zc != 0.f)
				{
					float z = 1.f / RayDir.Zc;
					RawPoints[4] = (+HalfSide - RayPos.Zc) * z;
					RawPoints[5] = (-HalfSide - RayPos.Zc) * z;
				}
				else
				{
					RawPoints[4] = 0;
					RawPoints[5] = 0;
				}

				INT::uint8t am = 0;
				Math::Vector Buffer[6];
				for (float t : RawPoints) {
					Math::Vector p = LocalRay.pointAt(t);
					if (t > 0 and CheckLocalPointVolume(p))
					{
						Buffer[am] = _BaseRay.pointAt(t);
						am += 1;
					}
				}

				Helpfulness::Bound<Math::Vector> ReadyPoints{ (am) };
				for (INT::uint8t i = 0; i < am; i++)
					ReadyPoints[i] = Buffer[i];

				return ReadyPoints;
			}
		};

		class Plane : public Primitive {
			using Primitive::Primitive;

			Helpfulness::Bound<Math::Vector> getCrossPoints(const Ray& _BaseRay) const override {

				Ray LocalRay = _BaseRay.relateTo(Position, Rotation);
				Math::Vector
					RayDir = LocalRay.getDir(),
					RayPos = LocalRay.getPos();
				float Point = 0;
				float DirS = RayDir.Yc;
				if (RayDir.Xc != 0 and RayDir.Yc != 0 and RayDir.Zc != 0)
				{
					Point = ((Position.Xc + Position.Yc + Position.Zc) - (RayPos.Xc + RayPos.Yc + RayPos.Zc)) / DirS;
					if (Point > 0)
					{
						Helpfulness::Bound<Math::Vector> Res{ 1 };
						Res[0] = _BaseRay.pointAt(Point);
						return Res;
					}
				}
				Helpfulness::Bound<Math::Vector> Res{ 0 };
				return Res;
			}

			bool CheckLocalPointVolume(const Math::Vector&) const override {
				return false;
			}

			Math::Vector getNormAt(const Math::Vector& _Point) const override {
				return (Rotation * Math::Vector::Y_plus() * Rotation.conjugate()).N;
			}
		};

		class Sphere : public Primitive {
			using Primitive::Primitive;

			Helpfulness::Bound<Math::Vector> getCrossPoints(const Ray& _BaseRay) const override {
				Ray LocalRay = Ray{ _BaseRay.getPos() - Position, _BaseRay.getDir() };
				Math::Vector
					RayDir = LocalRay.getDir(),
					RayPos = LocalRay.getPos();
				float
					DS = Math::Vector::dot(RayDir, RayPos),
					S2 = Math::Vector::dot(RayPos, RayPos);
				float
					b = 2 * DS,
					c = S2 - HalfSide * HalfSide;
				float D = b * b - 4 * c;
				float RawPoints[2]{
					(-b + sqrtf(D)) * 0.5,
					(-b - sqrtf(D)) * 0.5
				};
				if (RawPoints[0] > 0 and RawPoints[1] > 0)
					return Helpfulness::Bound<Math::Vector>::from(_BaseRay.pointAt(RawPoints[0]), _BaseRay.pointAt(RawPoints[1]));
				if (RawPoints[0] > 0)
					return Helpfulness::Bound<Math::Vector>::from(_BaseRay.pointAt(RawPoints[0]));
				if (RawPoints[1] > 0)
					return Helpfulness::Bound<Math::Vector>::from(_BaseRay.pointAt(RawPoints[1]));
				return Helpfulness::Bound<Math::Vector>::Empty();
			}

			Math::Vector getNormAt(const Math::Vector& _Point) const override {
				return (_Point - Position).normalize();
			}

			bool CheckLocalPointVolume(const Math::Vector& _Point) const override {
				return _Point.SQUARED_LENGTH_() <= HalfSide * HalfSide + PRECISION__;
			}
		};
	}
}