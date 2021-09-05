/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once
#include <DirectXMath.h>
namespace Rendering
{
  struct Vertex {
  DirectX::XMFLOAT3 position;
  DirectX::XMFLOAT3 normal;
  DirectX::XMFLOAT2 texCoords;
  DirectX::XMFLOAT3 tangent;
  DirectX::XMFLOAT3 bitangent;
  };
}

/*
struct VS_INPUT
{
  float3 position : POSITION;
  float3 normal : NORMAL;
  float2 texCoords : TEXCOORDS;
  float3 tangent : TANGENT;
  float3 bitangent : BITANGENT;
};

*/