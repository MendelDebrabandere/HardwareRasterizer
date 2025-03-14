#pragma once

struct SDL_Window;
struct SDL_Surface;

namespace dae
{

	class Mesh;
	class Camera;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		void ToggleRotation();
		void ToggleFilteringMethod();

	private:
		void InitMeshes();

		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		std::vector<Mesh*> m_MeshPtrs{};
		Camera* m_pCamera{ nullptr };

		ID3D11SamplerState* m_pSamplerState{ nullptr };
		ID3D11Device* m_pDevice{ nullptr };
		ID3D11DeviceContext* m_pDeviceContext{ nullptr };
		IDXGISwapChain* m_pSwapChain{ nullptr };
		ID3D11Texture2D* m_pDepthStencilBuffer{ nullptr };
		ID3D11DepthStencilView* m_pDepthStencilView{ nullptr };
		ID3D11RenderTargetView* m_pRenderTargetView{ nullptr };
		ID3D11Resource* m_pRenderTargetBuffer{ nullptr };


		//DIRECTX
		HRESULT InitializeDirectX();
		//...

		enum class FilteringMethod
		{
			Point = 0,
			Linear = 1,
			Anisotropic = 2
		};

		FilteringMethod m_FilteringMethod{ 0 };

		void LoadSampleState(const D3D11_FILTER& filter, ID3D11Device* device);

	};
}
