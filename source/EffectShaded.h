#pragma once
#include "Effect.h"

namespace dae
{
	class Texture;

	class EffectShaded final : public Effect
	{
	public:
		EffectShaded(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~EffectShaded();

		// rule of 5 copypasta
		EffectShaded(const EffectShaded& other) = delete;
		EffectShaded(EffectShaded&& other) = delete;
		EffectShaded& operator=(const EffectShaded& other) = delete;
		EffectShaded& operator=(EffectShaded&& other) = delete;

		virtual void SetDiffuseMap(const Texture* texture) override;
		virtual void SetNormalMap(const Texture* texture) override;
		virtual void SetSpecularMap(const Texture* texture) override;
		virtual void SetGlossinessMap(const Texture* texture) override;
		virtual void SetWorldMatrix(const float* matrix) override;
		virtual void SetInverseViewMatrix(const float* matrix) override;


	private:
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{ nullptr };

		ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable{};
		ID3DX11EffectMatrixVariable* m_pInverseViewMatrixVariable{};

	};
}

