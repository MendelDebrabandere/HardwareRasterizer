#include "pch.h"
#include "EffectShaded.h"
#include "Texture.h"

using namespace dae;

EffectShaded::EffectShaded(ID3D11Device* pDevice, const std::wstring& assetFile)
	: Effect(pDevice, assetFile)
{
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

	m_pWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldMatrixVariable->IsValid())
		std::wcout << L"m_pWorldMatrixVariable not valid\n";

	m_pInverseViewMatrixVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
	if (!m_pInverseViewMatrixVariable->IsValid())
		std::wcout << L"m_pInverseViewMatrixVariable not valid\n";
}

EffectShaded::~EffectShaded()
{

}

void EffectShaded::SetDiffuseMap(const Texture* texture)
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(texture->GetSRV());
	delete texture;
}

void EffectShaded::SetNormalMap(const Texture* texture)
{
	if (m_pNormalMapVariable)
		m_pNormalMapVariable->SetResource(texture->GetSRV());
	delete texture;
}

void EffectShaded::SetSpecularMap(const Texture* texture)
{
	if (m_pSpecularMapVariable)
		m_pSpecularMapVariable->SetResource(texture->GetSRV());
	delete texture;
}

void EffectShaded::SetGlossinessMap(const Texture* texture)
{
	if (m_pGlossinessMapVariable)
		m_pGlossinessMapVariable->SetResource(texture->GetSRV());
	delete texture;
}

void dae::EffectShaded::SetWorldMatrix(const float* matrix)
{
	m_pWorldMatrixVariable->SetMatrix(matrix);
}

void dae::EffectShaded::SetInverseViewMatrix(const float* matrix)
{
	m_pInverseViewMatrixVariable->SetMatrix(matrix);
}
