#include "PARenderPCH.h"
#include "ShaderProgram.h"
#include "RenderEngine.h" //TODO(SETH) TEMPORARY! FIX DATA FLOW AND ACCESS

#include <string>

static Microsoft::WRL::ComPtr<ID3DBlob> ShaderBlob(std::wstring const& byteCodePath);
static Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader(ID3DBlob* d3dblob);
static Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader(ID3DBlob* d3dblob);
static Microsoft::WRL::ComPtr<ID3D11GeometryShader> GeoShader(ID3DBlob* d3dblob);
namespace Rendering
{

  void ShaderProgram::Bind(ID3D11DeviceContext* context) const
  {
  assert(context != nullptr);
  assert(mVertexShader != nullptr);
  assert(mPixelShader != nullptr);

  context->VSSetShader(mVertexShader.Get(), nullptr, 0);
  context->PSSetShader(mPixelShader.Get(), nullptr, 0);

  if(mGeoShader != nullptr)
    context->GSSetShader(mGeoShader.Get(), nullptr, 0);

  context->IASetInputLayout(mInputLayout.Get());
  }

  ShaderProgram* ShaderProgram::Create(char const* vs_path, char const* ps_path, char const* geo_path)
  {
  std::wstring w_vs_path(Utility::utf8ToUtf16(vs_path));
  std::wstring w_ps_path(Utility::utf8ToUtf16(ps_path));

  Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = ShaderBlob(w_vs_path);
  Microsoft::WRL::ComPtr<ID3DBlob> psBlob = ShaderBlob(w_ps_path);

  Microsoft::WRL::ComPtr<ID3D11VertexShader> vs = VertexShader(vsBlob.Get());
  Microsoft::WRL::ComPtr<ID3D11PixelShader> ps = PixelShader(psBlob.Get());

  Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

  // Create the input layout
  D3D11_INPUT_ELEMENT_DESC input_layout[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORDS",  0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BITANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

    { "TRANSFORM",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "TRANSFORM",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "TRANSFORM",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "TRANSFORM",  3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

    { "NORMALMATRIX",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "NORMALMATRIX",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "NORMALMATRIX",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "NORMALMATRIX",  3, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },


    { "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
  };


  ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
  HRESULT hr = device->CreateInputLayout(input_layout, static_cast<UINT>(_countof(input_layout)), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), inputLayout.ReleaseAndGetAddressOf());
  if (FAILED(hr)) { __debugbreak(); }

  Microsoft::WRL::ComPtr<ID3D11GeometryShader> geo = nullptr;
  if (geo_path != nullptr)
  {
      std::wstring w_ps_path(Utility::utf8ToUtf16(geo_path));
      Microsoft::WRL::ComPtr<ID3DBlob> geoBlob = ShaderBlob(w_vs_path);
      geo = GeoShader(geoBlob.Get());
  }

  static ProgramID id = 0;
  return new ShaderProgram(vs.Get(), ps.Get(), geo.Get(), inputLayout.Get(), id++, ps_path);
  }

  ShaderProgram::ShaderProgram(ID3D11VertexShader* vs, ID3D11PixelShader* ps, ID3D11GeometryShader* geo, ID3D11InputLayout* input_layout, ProgramID id, const char* ps_path) :
  mVertexShader(vs), mPixelShader(ps), mGeoShader(geo), mInputLayout(input_layout), mID(id), fragPath(ps_path)
  {
  }

}

static Microsoft::WRL::ComPtr<ID3DBlob> ShaderBlob(std::wstring const& byteCodePath)
{
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  HRESULT hr = D3DReadFileToBlob(byteCodePath.c_str(), &blob);
  //TODO(Seth)Handle Error
  if (blob == nullptr)
  {
  __debugbreak();
  }

  return blob;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader(ID3DBlob* d3dblob)
{
  assert(d3dblob != nullptr);

  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;

  ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
  device->CreateVertexShader(d3dblob->GetBufferPointer(), d3dblob->GetBufferSize(), nullptr, &vertexShader);
  //TODO(Seth)Handle Error
  if (vertexShader == nullptr)
  {
  __debugbreak();
  }

  return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11GeometryShader> GeoShader(ID3DBlob* d3dblob)
{
    assert(d3dblob != nullptr);

    Microsoft::WRL::ComPtr<ID3D11GeometryShader> gShader;

    ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
    device->CreateGeometryShader(d3dblob->GetBufferPointer(), d3dblob->GetBufferSize(), nullptr, &gShader);
    //TODO(Seth)Handle Error
    if (gShader == nullptr)
    {
        __debugbreak();
    }

    return gShader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader(ID3DBlob* d3dblob)
{
  assert(d3dblob != nullptr);

  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

  ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
  device->CreatePixelShader(d3dblob->GetBufferPointer(), d3dblob->GetBufferSize(), nullptr, &pixelShader);
  //TODO(Seth)Handle Error
  if (pixelShader == nullptr)
  {
  __debugbreak();
  }

  return pixelShader;
}
