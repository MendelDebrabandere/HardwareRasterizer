#include "pch.h"
#include "Effect.h"
#include "Texture.h"
#include <cassert>
//#include "Mesh.h"
//#include <functional>

using namespace dae;

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	m_pEffect = LoadEffect(pDevice, assetFile);

	m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pTechnique->IsValid())
		std::wcout << L"Technique not valid\n";

	m_pWorldViewProjMatrixVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pWorldViewProjMatrixVariable->IsValid())
		std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";

	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"m_pDiffuseMapVariable not valid!\n";

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
		std::wcout << L"m_pNormalMapVariable not valid!\n";

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
		std::wcout << L"m_pSpecularMapVariable not valid!\n";

	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
		std::wcout << L"m_pGlossinessMapVariable not valid!\n";

	m_pSamplerStateVariable = m_pEffect->GetVariableByName("gSamState")->AsSampler();
	if (!m_pSamplerStateVariable->IsValid())
		std::wcout << L"m_pSamplerStateVariable not valid\n";

	m_pWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldMatrixVariable->IsValid())
		std::wcout << L"m_pWorldMatrixVariable not valid\n";

	m_pInverseViewMatrixVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
	if (!m_pInverseViewMatrixVariable->IsValid())
		std::wcout << L"m_pInverseViewMatrixVariable not valid\n";

	//Create Vertex Layout
	static constexpr uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;



	//Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
	{
		assert(false); //or return
	}

	std::cout << "FILTERING METHOD: POINT\n";
}

Effect::~Effect()
{
	if (m_pTechnique) m_pTechnique->Release();
	if (m_pEffect) m_pEffect->Release();

	//if (m_pMatWorldViewProjVariable) m_pMatWorldViewProjVariable->Release();
	//if (m_pDiffuseMapVariable) m_pDiffuseMapVariable->Release();
}

ID3DX11Effect* Effect::GetEffect() const
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechnique() const
{
	return m_pTechnique;
}

ID3D11InputLayout* Effect::GetInputLayout() const
{
	return m_pInputLayout;
}

void Effect::SetDiffuseMap(const Texture* texture)
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(texture->GetSRV());
	delete texture;
}

void Effect::SetNormalMap(const Texture* texture)
{
	if (m_pNormalMapVariable)
		m_pNormalMapVariable->SetResource(texture->GetSRV());
	delete texture;
}

void Effect::SetSpecularMap(const Texture* texture)
{
	if (m_pSpecularMapVariable)
		m_pSpecularMapVariable->SetResource(texture->GetSRV());
	delete texture;
}

void Effect::SetGlossinessMap(const Texture* texture)
{
	if (m_pGlossinessMapVariable)
		m_pGlossinessMapVariable->SetResource(texture->GetSRV());
	delete texture;
}

void Effect::ToggleFilteringMethod(ID3D11Device* device)
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

	LoadSampleState(newFilter, device);
}

void Effect::SetWorldViewProjMatrix(const float* matrix)
{
	m_pWorldViewProjMatrixVariable->SetMatrix(matrix);
}

void dae::Effect::SetWorldMatrix(const float* matrix)
{
	m_pWorldMatrixVariable->SetMatrix(matrix);
}

void dae::Effect::SetInverseViewMatrix(const float* matrix)
{
	m_pInverseViewMatrixVariable->SetMatrix(matrix);
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

void Effect::LoadSampleState(const D3D11_FILTER& filter, ID3D11Device* device)
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
		std::wcout << L"m_pSampleState failed to load\n";


	result = m_pSamplerStateVariable->SetSampler(0, m_pSamplerState);
	if (FAILED(result))
		std::wcout << L"m_pSampleState failed to Change\n";
}
