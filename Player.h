#pragma once
#include "Vector.h"
#include "Misc.h"
#include "Bound.h"

namespace Seltrad {
	namespace Engine {

		class Player {
			Math::Vector
				Position,
				View;

			float
				Wideness,
				Highness,

				HorDif,
				VerDif;

			INT::uint16t
				PerX,
				PerY;
			float
				InvPerX,
				InvPerY;
		public:
			Player(
				Math::Vector _Pos,
				Math::Vector _ViewDirection,
				float _ScreenWideness, float _ScreenHighness,
				INT::uint16t _ScreenResX, INT::uint16t _SCreebResY
			) :
				Position{ _Pos },
				View{ _ViewDirection.normalize() },
				Wideness{ _ScreenWideness }, Highness{ _ScreenHighness },
				PerX{ _ScreenResX }, PerY{ _SCreebResY },
				InvPerX{ 1.f / PerX }, InvPerY{ 1.f / PerY },
				HorDif{ Math::cotf(Wideness * 0.5f) }, VerDif{ Math::cotf(Highness * 0.5f) }
			{

			}
			
			INT::uint16t getResX(void) const {
				return PerX;
			}
			INT::uint16t getResY(void) const {
				return PerY;
			}

			Math::Vector getPos(void) const {
				return Position;
			}
			Math::Vector getView(void) const {
				return View;
			}

			Helpfulness::Bound<Ray> CastRays(void) const {
				Math::Vector
					SideView = Math::Vector::cross(View, Math::Vector::Y_plus()).normalize(),
					UpView = Math::Vector::cross(SideView, View).normalize();
				Math::Vector
					Center = View + Position;
				Math::Vector
					LeftBottom = Center - VerDif * UpView - HorDif * SideView;

				Helpfulness::Bound<Ray> Rays{ (size_t)PerX * PerY };
				for (size_t iY = 0; iY < PerY; iY++)
					for (size_t iX = 0; iX < PerX; iX++)
						Rays[iY * PerX + iX] =
						Ray(Position, {
						LeftBottom + 2*HorDif * iX * InvPerX * SideView + 2*VerDif * iY * InvPerY * UpView -
						Position
							});
				return Rays;
			}

			Player& Move(float _dFor, float _dSide) {
				Position =
					Position +
					_dFor * Math::Vector{ View.Xc, 0, View.Zc } +
					_dSide * Math::Vector::cross(View, Math::Vector::Y_plus()).normalize();
				return *this;
			}

			Player& Turn(float _dX, float _dY) {
				Math::Quaternion right{ Math::Vector::cross(View, Math::Vector::Y_plus()), _dY };
				float
					c = Math::cosf(_dX),
					s = Math::sinf(_dX);
				View = (right * Math::Quaternion{
					 View.Xc * c + View.Zc * s,
					 View.Yc,
					-View.Xc * s + View.Zc * c,
					0
					}*right.conjugate()).N;
				return *this;
			}
		};
	}
}