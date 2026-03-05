#pragma once
#include "Player.h"
#include "Primitives.h"

namespace Seltrad {
	namespace Engine {

		struct Volumizer {
			Player MainPlayer;
			Primitive** Objects = nullptr;
			size_t Amount = 0;
			static constexpr float mindepth = 1000000;

			static constexpr size_t CountOfTasks = 32;
			mutable std::thread* RayThreads[CountOfTasks];

			Volumizer() :
				MainPlayer{
					{0.f,0.f,0.f},
					{1.5f,0.5f,1.5f},
					Math::PiDIV3,
					Math::PiDIV2,
					800,
					400}
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

				for (float x = -2; x < 2; x++)
					for (float z = -2; z < 2; z++)
						AddObject(new Sphere(
							{ x - 10, 1, z },
							0.4,
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

			Helpfulness::Bound<INT::uint32t> GetColoredScheme(void) const {
				Helpfulness::Bound<float> Brightness = GetBrightScheme();
				Helpfulness::Bound<INT::uint32t> Scheme{ Brightness.length() };

				for (size_t i = 0; i < Scheme.length(); i++)
				{
					INT::uint8t Canal = (INT::uint8t)(Brightness[i] * 255.f);
					Scheme[i] = (Canal << 24) + (Canal << 16) + (Canal << 8);
				}

				return Scheme;
			}

			void CheckRayByThread(const Ray* _Ray, float* _SchemeValue, const Math::Vector& _LightPoint) const {
				float mins = mindepth;
				Math::Vector minPoint{};
				Primitive* minObj = nullptr;
				for (size_t iObj = 0; iObj < Amount; iObj++)
				{
					Helpfulness::Bound<Math::Vector> Cross = Objects[iObj]->getCrossPoints(*_Ray);
					for (INT::uint8t i = 0; i < Cross.length(); i++)
					{
						float l = (Cross[i] - MainPlayer.getPos()).SQUARED_LENGTH_();
						if (mins > l)
						{
							mins = l;
							minPoint = Cross[i];
							minObj = Objects[iObj];
						}
					}
				}
				if (mins == mindepth)
					*_SchemeValue = 0;
				else
				{
					float dist = (_LightPoint - minPoint).SQUARED_LENGTH_();
					*_SchemeValue = Math::exp2f(-dist * 0.05) * (Math::Vector::dot(minObj->getNormAt(minPoint), (_LightPoint - minPoint).normalize()) + 1) * 0.5f;
				}
			}

			/*Helpfulness::Bound<float> GetBrightScheme(void) const {
				Helpfulness::Bound<Ray> Rays = MainPlayer.CastRays();
				Helpfulness::Bound<float> Scheme{ Rays.length() };

				Math::Vector LightPoint = { -10, -1, 0 };

				for (size_t iRay = 0; iRay < Rays.length(); iRay++) {
					float mins = mindepth;
					Math::Vector minPoint{};
					Primitive* minObj = nullptr;
					for (size_t iObj = 0; iObj < Amount; iObj++)
					{
						Helpfulness::Bound<Math::Vector> Cross = Objects[iObj]->getCrossPoints(Rays[iRay]);
						for (INT::uint8t i = 0; i < Cross.length(); i++)
						{
							float l = (Cross[i] - MainPlayer.getPos()).SQUARED_LENGTH_();
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
					{
						float dist = (LightPoint - minPoint).SQUARED_LENGTH_();
						Scheme[iRay] = Math::exp2f(-dist * 0.02) * (Math::Vector::dot(minObj->getNormAt(minPoint), (LightPoint - minPoint).normalize()) + 1) * 0.5f;
					}
				}

				return Scheme;
			}*/

			Helpfulness::Bound<float> GetBrightScheme(void) const {
				Helpfulness::Bound<Ray>& Rays = MainPlayer.CastRays();
				size_t Amount = Rays.length();
				Helpfulness::Bound<float> Scheme{ Amount };

				Math::Vector LightPoint = { -10, -1, 0 };
				for (size_t iThr = 0; iThr < CountOfTasks; iThr++)
				{
					RayThreads[iThr] = new std::thread([this, Amount, iThr, &Rays, &Scheme, LightPoint]() {
						for (size_t iRay = Amount / CountOfTasks * iThr; iRay < Amount / CountOfTasks * (iThr + 1); iRay++)
							CheckRayByThread(&Rays[iRay], &Scheme[iRay], LightPoint);
						});
				}
				for (size_t iThr = 0; iThr < CountOfTasks; iThr++)
				{
					RayThreads[iThr]->join();
					delete RayThreads[iThr];
				}
				for (size_t iThr = 0; iThr < CountOfTasks; iThr++)
				{

				}

				return Scheme;
			}
		};
	}
}