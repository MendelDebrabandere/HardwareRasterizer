#include "pch.h"
#include "Mesh.h"
#include "Effect.h"
#include "Texture.h"
#include <cassert>
#include "utils.h"

using namespace dae;

Mesh::Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& texturePath)
	:m_pEffect{ new Effect(pDevice, L"Resources/PosCol3D.fx") }
{
	InitMesh(pDevice, vertices, indices, texturePath);
}

dae::Mesh::Mesh(ID3D11Device* pDevice, const std::string& objectPath, const std::string& texturePath)
	:m_pEffect{ new Effect(pDevice, L"Resources/PosCol3D.fx") }
{
	std::vector<Vertex> vertices{};
	std::vector<uint32_t> indices{};
	Utils::ParseOBJ("Resources/vehicle.obj", vertices, indices);

	InitMesh(pDevice, vertices, indices, texturePath);
}

void dae::Mesh::InitMesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& texturePath)
{

	//Create Vertex buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices.data();

	HRESULT result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;


	//Create Index Buffer
	m_NumIndices = static_cast<uint32_t>(indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return;


	m_pEffect->SetDiffuseMap(Texture::LoadFromFile(texturePath, pDevice));
}

Mesh::~Mesh()
{
	delete m_pEffect;

	if (m_pVertexBuffer) m_pVertexBuffer->Release();

	if (m_pIndexBuffer) m_pIndexBuffer->Release();

	//if (m_pTechnique) m_pTechnique->Release();
}

void dae::Mesh::Update(const Timer* pTimer)
{
	if (m_IsRotating)
	{
		constexpr float rotationSpeed{ 1 };
		m_Rotation += pTimer->GetElapsed() * rotationSpeed;
		m_WorldMatrix = Matrix::CreateRotationY(m_Rotation) * m_StartWorldMatrix;
	}
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext) const
{
	//1. Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//2. Set Input Layout
	pDeviceContext->IASetInputLayout(m_pEffect->GetImputLayout());

	//3. Set VertexBuffer
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//4. Set IndexBuffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//5. Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}


}

void dae::Mesh::SetMatrix(const Matrix& matrix)
{
	m_WorldViewProjectionMatrix = m_WorldMatrix * matrix;
	m_pEffect->SetMatrix(reinterpret_cast<float*>(&m_WorldViewProjectionMatrix));
}

void dae::Mesh::ToggleRotation()
{
	m_IsRotating = !m_IsRotating;
}

void dae::Mesh::ToggleFilteringMethod(ID3D11Device* device)
{
	m_pEffect->ToggleFilteringMethod(device);
}


 