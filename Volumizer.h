#pragma once
#include "Player.h"
#include "Primitives.h"

namespace Seltrad {
	namespace Engine {

		struct Volumizer {
			Player MainPlayer;
			Primitive** Objects = nullptr;
			size_t Amount = 0;
			static constexpr float mindepth = 10000;

			Volumizer() :
				MainPlayer{
					{0.f,0.f,0.f},
					{1.5f,0.5f,1.5f},
					Math::PiDIV3,
					Math::PiDIV2,
					100,
					100}
			{
				AddObject(new Cube(
					{ 10.f, 0.f, 0.f },
					0.4f,
					{ 0.f, 0.f, 0.f }
				));
				AddObject(new Cube(
					{ 10.f, 0.f, 1.f },
					0.4f,
					{ 0.f, 0.f, 0.f }
				));
				AddObject(new Cube(
					{ 10.f, 0.f, -1.f },
					0.4f,
					{ 0.f, 0.f, 0.f }
				));
				AddObject(new Sphere(
					{ 9, 0.f,0.f },
					0.4f,
					{ 0,0,0 }
				));
			}

			Primitive* AddObject(Primitive* _Object) {
				Primitive** Temp = new Primitive*[Amount + 1];
				for (size_t i = 0; i < Amount; i++)
					Temp[i] = Objects[i];
				Temp[Amount] = _Object;
				delete[] Objects;
				Objects = Temp;
				Amount += 1;
				return Objects[Amount - 1];
			}

			Helpfulness::Bound<INT::uint32t> GetColoredScheme(void) {
				Helpfulness::Bound<float> Brightness = GetBrightScheme();
				Helpfulness::Bound<INT::uint32t> Scheme{ Brightness.length() };

				for (size_t i = 0; i < Scheme.length(); i++)
				{
					INT::uint8t Canal = (INT::uint8t)(Brightness[i] * 255.f);
					Scheme[i] = (Canal << 24) + (Canal << 16) + (Canal << 8);
				}

				return Scheme;
			}

			Helpfulness::Bound<float> GetBrightScheme(void) {
				Helpfulness::Bound<Ray> Rays = MainPlayer.CastRays();
				Helpfulness::Bound<float> Scheme{ Rays.length() };

				for (size_t iRay = 0; iRay < Rays.length(); iRay++) {
					float mins = mindepth;
					Math::Vector minPoint{};
					Primitive* minObj = nullptr;
					for (size_t iObj = 0; iObj < Amount; iObj++)
					{
						Helpfulness::Bound<Math::Vector> Cross = Objects[iObj]->getCrossPoints(Rays[iRay]);
						for (INT::uint8t i = 0; i < Cross.length(); i++)
						{
							float l = (Cross[i] - MainPlayer.getPos()).length();
							if (mins > l)
							{
								mins = l;
								minPoint = Cross[i];
								minObj = Objects[iObj];
							}
						}
					}
					if (mins == mindepth)
						Scheme[iRay] = 0;
					else
						Scheme[iRay] = Math::powf(Math::exp2f(-mins / 10) * Math::fmaxf(0.06, Math::Vector::dot(minObj->getNormAt(minPoint), (Math::Vector{ 5, 1, 3 } - minPoint).normalize())), 1);
				}

				return Scheme;
			}
		};
	}
}