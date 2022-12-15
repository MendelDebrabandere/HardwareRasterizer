#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	class Camera
	{
	public:
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle);

		void Initialize(float _aspectRatio, float _fovAngle = 90.f, Vector3 _origin = { 0.f,0.f,0.f });

		Matrix GetProjectionMatrix() const;

		Matrix GetViewMatrix() const;

		void Update(const Timer* pTimer);

	private:
		float nearClip{ 0.1f };
		float farClip{ 100.f };

		Vector3 origin{};
		float fovAngle{ 90.f };
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };
		float aspectRatio{};

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		Matrix projectionMatrix{};

		void CalculateViewMatrix();

		void CalculateProjectionMatrix();

		void DoKeyboardInput(float deltaTime, float moveSpeed);

		void DoMouseInput(float deltaTime, float moveSpeed, float mouseSens);

	};
}
