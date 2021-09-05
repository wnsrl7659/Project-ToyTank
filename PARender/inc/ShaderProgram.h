/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once
#include "PARenderAPI/IShaderProgram.h"
#include <memory>
#include <wrl/client.h>
#include <d3d11.h>

namespace Rendering {
  class ShaderProgram : public IShaderProgram
  {
  public:
  using SharedPtr = std::shared_ptr<ShaderProgram>;
  virtual ProgramID ID() const override { return mID; };
  virtual std::string FragmentShader() const override { return fragPath; };

  void Bind(ID3D11DeviceContext* context) const;

  static ShaderProgram* Create(char const* vs_path, char const* ps_path, char const* geo_path = nullptr);



  private:
      std::string fragPath = {};
  ShaderProgram(ID3D11VertexShader* vs, ID3D11PixelShader* ps, ID3D11GeometryShader* geo, ID3D11InputLayout* input_layout, ProgramID id, const char* ps_path);
  Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader = nullptr;
  Microsoft::WRL::ComPtr<ID3D11PixelShader>  mPixelShader = nullptr;
  Microsoft::WRL::ComPtr<ID3D11GeometryShader>  mGeoShader = nullptr;
  Microsoft::WRL::ComPtr<ID3D11InputLayout>  mInputLayout = nullptr;
  ProgramID mID;
  };
}
