#include "pch.h"
#include "EffectTransparent.h"
#include "Texture.h"

using namespace dae;

EffectTransparent::EffectTransparent(ID3D11Device* pDevice, const std::wstring& assetFile)
	: Effect(pDevice, assetFile)
{
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"m_pDiffuseMapVariable not valid!\n";

}

EffectTransparent::~EffectTransparent()
{

}

void EffectTransparent::SetDiffuseMap(const Texture* texture)
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(texture->GetSRV());
	delete texture;
}
