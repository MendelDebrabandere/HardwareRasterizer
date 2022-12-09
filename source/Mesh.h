#pragma once

namespace dae
{
	struct Vertex final
	{
		dae::Vector3 position;
		dae::ColorRGB color;
	};

	class Effect;

	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();

		// rule of 5 copypasta
		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;

		void Render(ID3D11DeviceContext* pDeviceContext) const;

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
	};
}


