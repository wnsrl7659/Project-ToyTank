/***************************************************************************/
/*!
\file   Texture.h
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

#ifndef _TEXTURE_H
#define _TEXTURE_H
#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class Texture : public Microsoft::WRL::ComPtr<ID3D11Texture2D>
{

public:

    /* core */
    Texture() = default;
    ~Texture() = default;
    void Generate(ID3D11Device* device, D3D11_TEXTURE2D_DESC& desc, D3D11_SUBRESOURCE_DATA* data = nullptr);
    void Generate(ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT format, UINT bindFlags, DXGI_SAMPLE_DESC sampleDesc,
        D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuAccessFlags = 0, UINT miscFlags = 0, D3D11_SUBRESOURCE_DATA* data = nullptr,
        UINT mipLevels = 1, UINT arraySize = 1);

private:

};

#endif