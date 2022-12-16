#include "pch.h"
#include "Mesh.h"
#include "Effect.h"
#include "Texture.h"
#include <cassert>

using namespace dae;

Mesh::Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& texturePath)
	:m_pEffect{ new Effect(pDevice, L"Resources/PosCol3D.fx") }
{
	//Create Vertex Layout
	static constexpr uint32_t numElements{ 3 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create Input Layout
	//Point
	D3DX11_PASS_DESC passDesc{};
	m_pTechnique = m_pEffect->GetTechniquePoint();
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
	{
		assert(false); //or return
	}

	m_pTechnique = m_pEffect->GetTechniqueLinear();
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
	{
		assert(false); //or return
	}

	m_pTechnique = m_pEffect->GetTechniqueAnisotropic();
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
	{
		assert(false); //or return
	}

	//Create Vertex buffer
	m_Vertices = vertices;
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices.data();

	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
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

	if (m_pInputLayout) m_pInputLayout->Release();

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

void dae::Mesh::TransformVertices(ID3D11Device* pDevice)
{
	
	// Transform vertices
	m_pVertexBuffer->Release();
	m_pVertexBuffer = nullptr;

	std::vector<Vertex> transformedVertices{ m_Vertices };
	uint32_t counter{ 0 };
	for (const Vertex& vtx : m_Vertices)
	{
		 Vector4 newVtx = m_WorldViewProjectionMatrix.TransformPoint({ vtx.position, 1.f });

		transformedVertices[counter].position = newVtx.GetXYZ();

		transformedVertices[counter].position.x /= newVtx.w;
		transformedVertices[counter].position.y /= newVtx.w;
		transformedVertices[counter].position.z /= newVtx.w;

		++counter;
	}
	//Create Vertex buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(transformedVertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = transformedVertices.data();

	HRESULT result{};
	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);

	if (FAILED(result))
		std::cout << "Failed to update vertices of mesh\n";
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, int FilteringMethod) const
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
	switch (FilteringMethod)
	{
	case 0:
	{
		D3DX11_TECHNIQUE_DESC techDesc{};
		m_pEffect->GetTechniquePoint()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pEffect->GetTechniquePoint()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
		break;
	}
	case 1:
	{
		D3DX11_TECHNIQUE_DESC techDesc{};
		m_pEffect->GetTechniqueLinear()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pEffect->GetTechniqueLinear()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
		break;
	}
	case 2:
	{
		D3DX11_TECHNIQUE_DESC techDesc{};
		m_pEffect->GetTechniqueAnisotropic()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pEffect->GetTechniqueAnisotropic()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
		break;
	}
	}

}

void dae::Mesh::SetMatrix(Matrix matrix)
{
	m_WorldViewProjectionMatrix = m_WorldMatrix * matrix;
	m_pEffect->SetMatrix(reinterpret_cast<float*>(&m_WorldViewProjectionMatrix));
}

void dae::Mesh::ToggleRotation()
{
	m_IsRotating = !m_IsRotating;
}
