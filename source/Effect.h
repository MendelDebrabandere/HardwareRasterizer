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
		ID3DX11EffectTechnique* GetTechnique() const;
		ID3D11InputLayout* GetImputLayout() const;

		void SetDiffuseMap(const Texture* pDiffuseTexture);
		void ToggleFilteringMethod();
		void SetMatrix(const float* matrix);

	private:
		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);


		ID3DX11Effect* m_pEffect{ nullptr };
		ID3DX11EffectTechnique* m_pTechniquePoint{ nullptr };
		ID3DX11EffectTechnique* m_pTechniqueLinear{ nullptr };
		ID3DX11EffectTechnique* m_pTechniqueAnisotropic{ nullptr };

		//Create Input Layout part
		ID3DX11EffectTechnique* m_pCurrentTechnique{ nullptr };
		ID3D11InputLayout* m_pInputLayout{ nullptr };


		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };

		enum class FilteringMethod
		{
			Point = 0,
			Linear = 1,
			Anisotropic = 2
		};

		FilteringMethod m_FilteringMethod{ 0 };
	};
}

