#include "pch.h"
#include "Mesh.h"
#include "Effect.h"
#include <cassert>
#include "utils.h"

using namespace dae;

Mesh::Mesh(ID3D11Device* pDevice, const std::string& objectPath, Effect* pEffect)
	:m_pEffect{ pEffect }
{
	std::vector<Vertex> vertices{};
	std::vector<uint32_t> indices{};
	Utils::ParseOBJ(objectPath, vertices, indices);

	m_pInputLayout = m_pEffect->LoadInputLayout(pDevice);

	InitMesh(pDevice, vertices, indices);
}

void Mesh::InitMesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
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
}

Mesh::~Mesh()
{
	delete m_pEffect;

	if (m_pVertexBuffer) m_pVertexBuffer->Release();

	if (m_pIndexBuffer) m_pIndexBuffer->Release();
}

void Mesh::Update(const Timer* pTimer)
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
	pDeviceContext->IASetInputLayout(m_pInputLayout);

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

void Mesh::SetMatrix(const Matrix& matrix, Matrix* invViewMatrix)
{
	m_WorldViewProjectionMatrix = m_WorldMatrix * matrix;
	m_pEffect->SetWorldViewProjMatrix(reinterpret_cast<float*>(&m_WorldViewProjectionMatrix));
	m_pEffect->SetWorldMatrix(reinterpret_cast<float*>(&m_WorldMatrix));
	m_pEffect->SetInverseViewMatrix(reinterpret_cast<float*>(invViewMatrix));
}

void Mesh::ToggleRotation()
{
	m_IsRotating = !m_IsRotating;
}

void Mesh::SetSamplerState(ID3D11SamplerState* pSampleState)
{
	m_pEffect->SetSampleState(pSampleState);
}


 