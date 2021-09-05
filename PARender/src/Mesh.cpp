#include "PARenderPCH.h"
#include "Mesh.h"

#include "RenderEngine.h" //TODO(SETH) TEMPORARY -> MAKE AN ALLOCATOR
#include <cassert>

namespace Rendering {
  namespace Assets {

  Mesh::SharedPtr Mesh::Create(std::vector<Rendering::Vertex> const& vertices, std::vector<unsigned> const& indices)
  {
    RenderEngine* engine = reinterpret_cast<RenderEngine*>(RenderEngine::GetInstance());
    ID3D11Device* device = engine->Device();

    assert(device != nullptr);

    // Create an initialize the vertex buffer.
    D3D11_BUFFER_DESC vertexBufferDesc{};
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA vertdata{};
    vertdata.pSysMem = &vertices[0];

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertbuff;

    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertdata, vertbuff.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
    __debugbreak();
    }

    // Create an initialize the index buffer.
    D3D11_BUFFER_DESC indexBufferDesc{};
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(UINT));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA indexdata{};
    indexdata.pSysMem = &indices[0];

    Microsoft::WRL::ComPtr<ID3D11Buffer> indexbuff;

    hr = device->CreateBuffer(&indexBufferDesc, &indexdata, indexbuff.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
    __debugbreak();
    }

    return Mesh::SharedPtr(new Mesh(vertbuff.Get(), indexbuff.Get(), static_cast<UINT>(vertices.size()), static_cast<UINT>(indices.size())));
  }

  Mesh::Mesh(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, UINT vertCount, UINT indexCount) :
    mVertexBuffer(vertexBuffer), mIndexBuffer(indexBuffer), mVertCount(vertCount), mIndexCount(indexCount)
  {
  }

  void Mesh::Draw() const
  {
    RenderEngine* engine = reinterpret_cast<RenderEngine*>(RenderEngine::GetInstance());
    ID3D11Device* device = engine->Device();
    assert(device != nullptr);
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(context.ReleaseAndGetAddressOf());
    assert(context != nullptr);

    const UINT vertStride = sizeof(Vertex);
    const UINT offset = 0;

    //context->PSSetConstantBuffers(0, 1, mMaterials.front().cBuffer.GetAddressOf());

    context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &vertStride, &offset);
    context->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->DrawIndexed(mIndexCount, 0, 0);
  }

  ID3D11Buffer* Mesh::GetVertexBuffer(UINT& count) const
  {
    if (mVertexBuffer != nullptr)
    {
    count = mVertCount;
    return mVertexBuffer.Get();
    }

    count = -1;
    return nullptr;
  }

  ID3D11Buffer* Mesh::GetIndexBuffer(UINT& count) const
  {
    if (mIndexBuffer != nullptr)
    {
    count = mIndexCount;
    return mIndexBuffer.Get();
    }

    count = -1;
    return nullptr;
  }

  }
}
