#include "Crate.h"
#include <DirectXMath.h>
#include "GeometryGenerator.h"
#include <DirectXColors.h>
#include "DDSTextureLoader.h"
#include "ShaderData.h"

Crate::Crate(Renderer* renderer) : m_Renderer(renderer)
{
}

bool Crate::Load()
{
    Geometry::CreateBox(1.0f, 1.0f, 1.0f, &m_MeshData);

    // Create vertex buffer
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = (UINT)(sizeof(Vertex) * m_MeshData.vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vInitData = {};
    vInitData.pSysMem = &m_MeshData.vertices[0];

    DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&vbd, &vInitData, &m_VertexBuffer));

    // Create index buffer
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = (UINT)(sizeof(UINT) * m_MeshData.indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iInitData = {};
    iInitData.pSysMem = &m_MeshData.indices[0];

    DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&ibd, &iInitData, &m_IndexBuffer));
    
    // Load texture
    ID3D11Resource* resource = nullptr;
    DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), L"Textures\\crate_diffuse.dds", &resource, &m_DiffuseTexture));
    resource->Release();
    
    DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), L"Textures\\crate_normal.dds", &resource, &m_NormalTexture));
    resource->Release();

	return true;
}

void Crate::Render(Camera* camera)
{
    // Bind the vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    m_Renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    // Bind the index buffer
    m_Renderer->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set topology
    m_Renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set world buffer
    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
    ConstantBuffer cb;
    cb.mWorld = DirectX::XMMatrixTranspose(world);
    cb.mView = DirectX::XMMatrixTranspose(camera->GetView());
    cb.mProjection = DirectX::XMMatrixTranspose(camera->GetProjection());
    cb.mWorldInverse = DirectX::XMMatrixInverse(nullptr, world);

    Material material;
    material.mDiffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.mAmbient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.mSpecular = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    cb.mMaterial = material;

    ID3D11Buffer* worldConstantBuffer = m_Renderer->GetWorldConstantBuffer();
    m_Renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &worldConstantBuffer);
    m_Renderer->GetDeviceContext()->PSSetConstantBuffers(0, 1, &worldConstantBuffer);
    m_Renderer->GetDeviceContext()->UpdateSubresource(worldConstantBuffer, 0, nullptr, &cb, 0, 0);

    m_Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &m_DiffuseTexture);
    m_Renderer->GetDeviceContext()->PSSetShaderResources(1, 1, &m_NormalTexture);

    // Render geometry
    m_Renderer->GetDeviceContext()->DrawIndexed((UINT)m_MeshData.indices.size(), 0, 0);
}