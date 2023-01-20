#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "EffectShaded.h"
#include "EffectTransparent.h"
#include "Utils.h"
#include "Texture.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		InitMeshes();

		m_pCamera = new Camera();
		m_pCamera->Initialize(float(m_Width) / m_Height, 45.f, { 0,0,-50.f });

	}

	Renderer::~Renderer()
	{
		delete m_pCamera;
		for (Mesh* pMesh : m_MeshPtrs)
		{
			delete pMesh;
		}

		if (m_pRenderTargetView) m_pRenderTargetView->Release();
		if (m_pRenderTargetBuffer) m_pRenderTargetBuffer->Release();

		if (m_pDepthStencilView) m_pDepthStencilView->Release();
		if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();

		if (m_pSwapChain) m_pSwapChain->Release();

		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		if (m_pDevice) m_pDevice->Release();
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pCamera->Update(pTimer);
		for (Mesh* pMesh : m_MeshPtrs)
		{
			pMesh->SetMatrix(m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix(), m_pCamera->GetInvViewMatrix());

			pMesh->Update(pTimer);
		}
	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;


		//1. CLEAR RTV & DSV
		constexpr ColorRGB clearColor{ 0.f,0.f,0.3f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);


		//2. SET PIPELINE + INVOKE DRAWCALLS (= RENDER)
		for (Mesh* pMesh : m_MeshPtrs)
		{
			pMesh->Render(m_pDeviceContext);
		}




		//3. PRESENT BACKBUFFER (SWAP)
		m_pSwapChain->Present(0, 0);

	}

	void Renderer::ToggleRotation()
	{
		for (Mesh* pMesh : m_MeshPtrs)
		{
			pMesh->ToggleRotation();
		}
	}

	void Renderer::ToggleFilteringMethod()
	{
		D3D11_FILTER newFilter{};
		switch (m_FilteringMethod)
		{
		case FilteringMethod::Point:
			m_FilteringMethod = FilteringMethod::Linear;
			newFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			std::cout << "FILTERING METHOD: LINEAR\n";
			break;
		case FilteringMethod::Linear:
			m_FilteringMethod = FilteringMethod::Anisotropic;
			newFilter = D3D11_FILTER_ANISOTROPIC;
			std::cout << "FILTERING METHOD: ANISOTROPIC\n";
			break;
		case FilteringMethod::Anisotropic:
			m_FilteringMethod = FilteringMethod::Point;
			newFilter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			std::cout << "FILTERING METHOD: POINT\n";
			break;
		}

		LoadSampleState(newFilter, m_pDevice);
	}

	void Renderer::InitMeshes()
	{
		//Vehicle
		EffectShaded* vehicleEffect{ new EffectShaded{ m_pDevice, L"Resources/PosCol3D.fx" } };

		//Load textures
		Texture* pDiffuse{ Texture::LoadFromFile("Resources/vehicle_diffuse.png", m_pDevice) };
		vehicleEffect->SetDiffuseMap(pDiffuse);

		Texture* pNormal{ Texture::LoadFromFile("Resources/vehicle_normal.png", m_pDevice) };
		vehicleEffect->SetNormalMap(pNormal);

		Texture* pSpecular{ Texture::LoadFromFile("Resources/vehicle_specular.png", m_pDevice) };
		vehicleEffect->SetSpecularMap(pSpecular);

		Texture* pGlossiness{ Texture::LoadFromFile("Resources/vehicle_gloss.png", m_pDevice) };
		vehicleEffect->SetGlossinessMap(pGlossiness);

		//The Set...Map function autiomatically deletes the texture so no need to delete them here

		//Create vehicle
		Mesh* pVehicle{ new Mesh{ m_pDevice, "Resources/vehicle.obj", vehicleEffect} };
		m_MeshPtrs.push_back(pVehicle);



		//Fire
		EffectTransparent* fireEffect{ new EffectTransparent{ m_pDevice, L"Resources/PartialCoverage.fx" } };

		//Load textures
		Texture* pFireDiffuse{ Texture::LoadFromFile("Resources/fireFX_diffuse.png", m_pDevice) };
		fireEffect->SetDiffuseMap(pFireDiffuse);

		//The Set...Map function autiomatically deletes the texture so no need to delete them here

		//Create fire
		Mesh* pFire{ new Mesh{ m_pDevice, "Resources/fireFX.obj", fireEffect} };
		m_MeshPtrs.push_back(pFire);





	}

	HRESULT Renderer::InitializeDirectX()
	{
		//1. Create Device & DeviceContext
		//=====
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel,
											1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

		if (FAILED(result))
			return result;

		//Create DXGI Factory
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;



		//2. Create Swapchain
		//=====
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		// Get the handle (HWND) from the SDL Backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_VERSION(&sysWMInfo.version)
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		//Create SwapChain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
			return result;

		//3. Create DepthStencil (DS) & DepthStencilView (DSV)
		//Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;


		//4. Create RenderTarget (RT) & RenderTargetView (RTV)
		//=====

		//Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		//View
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;


		//5. Bind RTV & DSV to Output Merger Stage
		//=====
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


		//6. Set Viewport
		//=====
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		return result;

	}

	void Renderer::LoadSampleState(const D3D11_FILTER& filter, ID3D11Device* device)
	{
		// Create the SampleState description
		D3D11_SAMPLER_DESC sampleDesc{};
		sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampleDesc.MipLODBias = 0;
		sampleDesc.MinLOD = 0;
		sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
		sampleDesc.MaxAnisotropy = 16;
		sampleDesc.Filter = filter;


		if (m_pSamplerState) m_pSamplerState->Release();


		HRESULT result{ device->CreateSamplerState(&sampleDesc, &m_pSamplerState) };
		if (FAILED(result))
			std::cout << "m_pSamplerState failed to load\n";

		for (Mesh* pMesh : m_MeshPtrs)
		{
			pMesh->SetSamplerState(m_pSamplerState);
		}

	}

}
