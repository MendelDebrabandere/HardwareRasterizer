#pragma once

namespace dae
{
	class Texture;

	class Effect final
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~Effect();

		// rule of 5 copypasta
		Effect(const Effect& other) = delete;
		Effect(Effect&& other) = delete;
		Effect& operator=(const Effect& other) = delete;
		Effect& operator=(Effect&& other) = delete;

		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechniquePoint() const;
		ID3DX11EffectTechnique* GetTechniqueLinear() const;
		ID3DX11EffectTechnique* GetTechniqueAnisotropic() const;
		void SetDiffuseMap(Texture* pDiffuseTexture);

		void SetMatrix(const float* matrix);
	private:
		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

		ID3DX11Effect* m_pEffect{ nullptr };
		ID3DX11EffectTechnique* m_pTechniquePoint{ nullptr };
		ID3DX11EffectTechnique* m_pTechniqueLinear{ nullptr };
		ID3DX11EffectTechnique* m_pTechniqueAnisotropic{ nullptr };

		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
	};
}

