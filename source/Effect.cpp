#include "pch.h"
#include "Effect.h"

#include <cassert>

namespace dae
{
	Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
	{
		m_pEffect = LoadEffect(pDevice, assetFile);

		m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
		if (!m_pTechnique->IsValid())
			std::wcout << L"Technique not valid\n";

		m_pWorldViewProjMatrixVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		if (!m_pWorldViewProjMatrixVariable->IsValid())
			std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";

		m_pSamplerStateVariable = m_pEffect->GetVariableByName("gSamState")->AsSampler();
		if (!m_pSamplerStateVariable->IsValid())
			std::wcout << L"m_pSamplerStateVariable not valid\n";
	}

	Effect::~Effect()
	{
		if (m_pEffect) m_pEffect->Release();
	}

	void Effect::SetWorldViewProjMatrix(const float* matrix)
	{
		m_pWorldViewProjMatrixVariable->SetMatrix(matrix);
	}

	ID3DX11Effect* Effect::GetEffect() const
	{
		return m_pEffect;
	}

	ID3DX11EffectTechnique* Effect::GetTechnique() const
	{
		return m_pTechnique;
	}

	ID3D11InputLayout* Effect::LoadInputLayout(ID3D11Device* pDevice)
	{
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

		ID3D11InputLayout* pInputLayout;

		HRESULT result = pDevice->CreateInputLayout(
			vertexDesc,
			numElements,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&pInputLayout);

		if (FAILED(result))
		{
			assert(false); //or return
		}

		return pInputLayout;
	}

	void Effect::SetSampleState(ID3D11SamplerState* pSampleState)
	{
		HRESULT hr{ m_pSamplerStateVariable->SetSampler(0, pSampleState) };
		if (FAILED(hr)) 
			std::wcout << L"Failed to set sample state";
	}

	ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile) const
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{ nullptr };
		ID3DX11Effect* pEffect{};

		DWORD shaderFlags{ 0 };

#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// Load the effect form the file
		result = D3DX11CompileEffectFromFile
		(
			assetFile.c_str(),
			nullptr,
			nullptr,
			shaderFlags,
			0,
			pDevice,
			&pEffect,
			&pErrorBlob
		);

		// If loading the effect failed, print an error message
		if (FAILED(result))
		{
			if (pErrorBlob != nullptr)
			{
				const char* pErrors{ static_cast<char*>(pErrorBlob->GetBufferPointer()) };

				std::wstringstream ss;
				for (unsigned int i{}; i < pErrorBlob->GetBufferSize(); ++i)
				{
					ss << pErrors[i];
				}

				OutputDebugStringW(ss.str().c_str());
				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << "\n";
			}
			else
			{
				std::wstringstream ss;
				ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
				std::wcout << ss.str() << "\n";
				return nullptr;
			}
		}

		return pEffect;
	}
}

