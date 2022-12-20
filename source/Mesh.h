#pragma once

namespace dae
{
	struct Vertex final
	{
		dae::Vector3 position;
		dae::Vector2 uv;
	};

	class Effect;

	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& texturePath);
		~Mesh();

		// rule of 5 copypasta
		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;

		void Update(const Timer* pTimer);
		void Render(ID3D11DeviceContext* pDeviceContext, int FilteringMethod) const;
		void SetMatrix(Matrix matrix);
		void ToggleRotation();

	private:
		//Create Vertex Layout part
		Effect* m_pEffect{ nullptr };

		//Create Input Layout part
		ID3DX11EffectTechnique* m_pTechnique{ nullptr };
		ID3D11InputLayout* m_pInputLayout{ nullptr };

		//Create Vertex buffer part
		ID3D11Buffer* m_pVertexBuffer{ nullptr };

		//Create Index Buffer part
		uint32_t m_NumIndices{};
		ID3D11Buffer* m_pIndexBuffer{ nullptr };

		const Matrix m_StartWorldMatrix{ Matrix::CreateTranslation(0,0,0) };
		Matrix m_WorldMatrix{};
		Matrix m_WorldViewProjectionMatrix{};

		bool m_IsRotating{ false };
		float m_Rotation{};
	};
}


