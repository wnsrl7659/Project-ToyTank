/***************************************************************************/
/*!
\file   DepthStencilView.h
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

#ifndef _DEPTHSTENCILVIEW_H
#define _DEPTHSTENCILVIEW_H
#pragma once

#include "Texture.h"

class DepthStencilView : public Microsoft::WRL::ComPtr<ID3D11DepthStencilView>
{
public:

    /* core */
    DepthStencilView() = default;
    ~DepthStencilView() = default;
    void Generate(ID3D11Device* device, const Texture buffer, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc);
    void Clear(ID3D11DeviceContext* context, const FLOAT depth, const UINT8 stencil);
    void ClearDepth(ID3D11DeviceContext* context, const FLOAT depth);
    void ClearStencil(ID3D11DeviceContext* context, const UINT8 stencil);

    Texture GetTextureComPtr() const;
    ID3D11Texture2D* GetTexture() const;
    ID3D11Texture2D** GetAddressOfTexture();
    ID3D11Texture2D* const* GetAddressOfTexture() const;
private:

    /* d3d objects */
    Texture depthBuffer_;
};

#endif