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

	m_pTechniquePoint = m_pEffect->GetTechniqueByName("PointTechnique");
	if (!m_pTechniquePoint->IsValid())
		std::wcout << L"Technique not valid\n";
	m_pTechniqueLinear = m_pEffect->GetTechniqueByName("LinearTechnique");
	if (!m_pTechniqueLinear->IsValid())
		std::wcout << L"Technique not valid\n";
	m_pTechniqueAnisotropic = m_pEffect->GetTechniqueByName("AnisotropicTechnique");
	if (!m_pTechniqueAnisotropic->IsValid())
		std::wcout << L"Technique not valid\n";

	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";
	}

	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseMapVariable not valid!\n";
	}


	//Create Vertex Layout
	static constexpr uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create Input Layout
	//Point
	D3DX11_PASS_DESC passDesc{};
	m_pTechniqueLinear->GetPassByIndex(0)->GetDesc(&passDesc);

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

	//Linear
	m_pTechniqueLinear->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
	{
		assert(false); //or return
	}

	//Anisotropic
	m_pTechniqueAnisotropic->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
	{
		assert(false); //or return
	}


	

	m_pCurrentTechnique = m_pTechniquePoint;
	std::cout << "FILTERING METHOD: POINT\n";
}

Effect::~Effect()
{
	if (m_pTechniquePoint) m_pTechniquePoint->Release();
	if (m_pTechniqueLinear) m_pTechniqueLinear->Release();
	if (m_pTechniqueAnisotropic) m_pTechniqueAnisotropic->Release();
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
	return m_pCurrentTechnique;
}

ID3D11InputLayout* dae::Effect::GetImputLayout() const
{
	return m_pInputLayout;
}

void Effect::SetDiffuseMap(const Texture* pDiffuseTexture)
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());
	delete pDiffuseTexture;
}

void dae::Effect::ToggleFilteringMethod()
{
	switch (m_FilteringMethod)
	{
	case FilteringMethod::Point:
		m_FilteringMethod = FilteringMethod::Linear;
		m_pCurrentTechnique = m_pTechniqueLinear;
		std::cout << "FILTERING METHOD: LINEAR\n";
		break;
	case FilteringMethod::Linear:
		m_FilteringMethod = FilteringMethod::Anisotropic;
		m_pCurrentTechnique = m_pTechniqueAnisotropic;
		std::cout << "FILTERING METHOD: ANISOTROPIC\n";
		break;
	case FilteringMethod::Anisotropic:
		m_FilteringMethod = FilteringMethod::Point;
		m_pCurrentTechnique = m_pTechniquePoint;
		std::cout << "FILTERING METHOD: POINT\n";
		break;
	}
}

void dae::Effect::SetMatrix(const float* matrix)
{
	m_pMatWorldViewProjVariable->SetMatrix(matrix);
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
