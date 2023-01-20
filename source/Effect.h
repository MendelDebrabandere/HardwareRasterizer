#pragma once
namespace dae
{
	class Texture;

	class Effect
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~Effect();

		// rule of 5 copypasta
		Effect(const Effect& other) = delete;
		Effect(Effect&& other) = delete;
		Effect& operator=(const Effect& other) = delete;
		Effect& operator=(Effect&& other) = delete;

		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;
		ID3D11InputLayout* LoadInputLayout(ID3D11Device* pDevice);

		void SetWorldViewProjMatrix(const float* matrix);

		// pure virtuals
		virtual void SetDiffuseMap(const Texture* texture) = 0;
		virtual void SetNormalMap(const Texture* texture) = 0;
		virtual void SetSpecularMap(const Texture* texture) = 0;
		virtual void SetGlossinessMap(const Texture* texture) = 0;
		virtual void SetWorldMatrix(const float* matrix) = 0;
		virtual void SetInverseViewMatrix(const float* matrix) = 0;

		void SetSampleState(ID3D11SamplerState* pSampleState);
	protected:
		ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile) const;


		ID3DX11Effect* m_pEffect{ nullptr };

		//Create Input Layout part
		ID3DX11EffectTechnique* m_pTechnique{ nullptr };

		ID3DX11EffectSamplerVariable* m_pSamplerStateVariable{ nullptr };
		ID3DX11EffectMatrixVariable* m_pWorldViewProjMatrixVariable{ nullptr };

	};
}

