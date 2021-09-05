/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <memory>

#include "Vertex.h"

namespace Rendering {
  namespace Assets {
  class Mesh : public std::enable_shared_from_this<Mesh>
  {
  public:
  using SharedPtr = std::shared_ptr<Mesh>;
  
  void Draw() const;
  ID3D11Buffer* GetVertexBuffer(UINT& count) const;
  ID3D11Buffer* GetIndexBuffer(UINT& count) const;

  static Mesh::SharedPtr Create(std::vector<Rendering::Vertex>const&, std::vector<unsigned>const&);
  private:
  Mesh(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, UINT vertCount, UINT indexCount);
  Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
  UINT mVertCount;
  UINT mIndexCount;
  };

}} //namespace Rendering::Assets
