#include "pch.h"
#include "Camera.h"

namespace dae
{
	Camera::Camera(const Vector3& _origin, float _fovAngle)
		:origin { _origin },
		fovAngle{ _fovAngle }
		{
			CalculateProjectionMatrix();
		}

	void Camera::Initialize(float _aspectRatio, float _fovAngle, Vector3 _origin)
	{
		aspectRatio = _aspectRatio;
		fovAngle = _fovAngle;
		fov = tanf((fovAngle * TO_RADIANS) / 2.f);

		origin = _origin;
		forward = Vector3::UnitZ;
	}

	void dae::Camera::CalculateViewMatrix()
	{
		right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
		up = Vector3::Cross(forward, right);

		Matrix invViewMatrix { right,up,forward,origin };

		viewMatrix = invViewMatrix.Inverse();

		//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
		//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
	}

	void dae::Camera::CalculateProjectionMatrix()
	{
		projectionMatrix = Matrix{ Vector4{ 1 / (aspectRatio * fov), 0, 0, 0 },
									Vector4{ 0, 1 / fov, 0, 0 },
									Vector4{ 0,0,farClip / (farClip - nearClip), 1},
									Vector4{ 0,0,-(farClip * nearClip) / (farClip - nearClip), 0} };

		//const float Sw{ 2 * sqrtf(((nearClip * nearClip) / (cosf(fov) * cosf(fov)) - (nearClip * nearClip))) };
		//const float Sh{ 2 * sqrtf(((nearClip * nearClip) / (cosf(fov * aspectRatio) * cosf(fov * aspectRatio)) - (nearClip * nearClip))) };

		//projectionMatrix = Matrix{	Vector4{ 2 * nearClip / Sw,	0,					0,								0												},
		//							Vector4{ 0,					2 * nearClip / Sh,	0,								0												},
		//							Vector4{ 0,					0,					farClip / (farClip - nearClip), 0												},
		//							Vector4{ 0,					0,					0,								((-farClip) * nearClip) / farClip - nearClip}	};

		//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
		//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
	}

	Matrix Camera::GetProjectionMatrix() const
	{
		return projectionMatrix;
	}

	Matrix dae::Camera::GetViewMatrix() const
	{
		return viewMatrix;
	}

	void dae::Camera::Update(const Timer* pTimer)
	{
		//Camera Update Logic
		//...

		const float deltaTime = pTimer->GetElapsed();

		constexpr float movementSpeed = 13.f;
		constexpr float mouseSens = 0.006f;

		DoKeyboardInput(deltaTime, movementSpeed);

		DoMouseInput(deltaTime, movementSpeed, mouseSens);

		const Matrix finalRotation{ Matrix::CreateRotation(totalPitch, totalYaw, 0) };

		forward = finalRotation.TransformVector(Vector3::UnitZ);
		forward.Normalize();




		//Update Matrices
		CalculateViewMatrix();
		CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
	}

	void dae::Camera::DoKeyboardInput(float deltaTime, float moveSpeed)
	{
		//Keyboard Input
		const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
		if (pKeyboardState[SDL_SCANCODE_W])
		{
			origin += moveSpeed * deltaTime * forward;
		}
		if (pKeyboardState[SDL_SCANCODE_S])
		{
			origin -= moveSpeed * deltaTime * forward;
		}
		if (pKeyboardState[SDL_SCANCODE_A])
		{
			origin -= moveSpeed * deltaTime * right;
		}
		if (pKeyboardState[SDL_SCANCODE_D])
		{
			origin += moveSpeed * deltaTime * right;
		}
		if (pKeyboardState[SDL_SCANCODE_Q])
		{
			origin.y -= moveSpeed * deltaTime;
		}
		if (pKeyboardState[SDL_SCANCODE_E])
		{
			origin.y += moveSpeed * deltaTime;
		}
	}

	void dae::Camera::DoMouseInput(float deltaTime, float moveSpeed, float mouseSens)
	{
		//Mouse Input
		int mouseX{}, mouseY{};
		const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

		if (mouseState & SDL_BUTTON_RMASK && mouseState & SDL_BUTTON_LMASK)
		{
			origin.y -= moveSpeed * deltaTime * mouseY;
		}
		else if (mouseState & SDL_BUTTON_RMASK)
		{
			totalYaw += mouseSens * mouseX;
			totalPitch -= mouseSens * mouseY;
			if (abs(totalPitch) >= float(M_PI) / 2.f)
			{
				if (totalPitch < 0)
				{
					totalPitch = -float(M_PI) / 2.001f;
				}
				else
				{
					totalPitch = float(M_PI) / 2.001f;
				}
			}
		}
		else if (mouseState & SDL_BUTTON_LMASK)
		{
			totalYaw += mouseSens * mouseX;
			this->origin -= moveSpeed * deltaTime * this->forward * float(mouseY);
		}
	}
}
