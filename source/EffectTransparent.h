#pragma once
#include "Effect.h"

namespace dae
{
	class Texture;

	class EffectTransparent final : public Effect
	{
	public:
		EffectTransparent(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~EffectTransparent();

		// rule of 5 copypasta
		EffectTransparent(const EffectTransparent& other) = delete;
		EffectTransparent(EffectTransparent&& other) = delete;
		EffectTransparent& operator=(const EffectTransparent& other) = delete;
		EffectTransparent& operator=(EffectTransparent&& other) = delete;

		virtual void SetDiffuseMap(const Texture* texture) override;

		//empty funcitons
		virtual void SetNormalMap(const Texture* texture) override {};
		virtual void SetSpecularMap(const Texture* texture) override {};
		virtual void SetGlossinessMap(const Texture* texture) override {};
		virtual void SetWorldMatrix(const float* matrix) override {};
		virtual void SetInverseViewMatrix(const float* matrix) override {};

	private:
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
		
	};
}

