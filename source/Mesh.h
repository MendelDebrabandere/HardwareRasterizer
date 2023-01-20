#pragma once

namespace dae
{
	struct Vertex final
	{
		Vector3 position;
		Vector2 uv;
		Vector3 normal;
		Vector3 tangent;
	};

	class Effect;
	class Texture;

	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, const std::string& objectPath, Effect* pEffect);
		~Mesh();

		// rule of 5 copypasta
		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;

		void Update(const Timer* pTimer);
		void Render(ID3D11DeviceContext* pDeviceContext) const;
		void SetMatrix(const Matrix& matrix, Matrix* invViewMatrix);
		void ToggleRotation();
		void SetSamplerState(ID3D11SamplerState* pSampleState);

	private:
		void InitMesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);


		Effect* m_pEffect{ nullptr };
		ID3D11InputLayout* m_pInputLayout{ nullptr };
		ID3D11Buffer* m_pVertexBuffer{ nullptr };

		uint32_t m_NumIndices{};
		ID3D11Buffer* m_pIndexBuffer{ nullptr };

		const Matrix m_StartWorldMatrix{ Matrix::CreateTranslation(0,0,0) };
		Matrix m_WorldMatrix{};
		Matrix m_WorldViewProjectionMatrix{};

		bool m_IsRotating{ false };
		float m_Rotation{};
	};
}


