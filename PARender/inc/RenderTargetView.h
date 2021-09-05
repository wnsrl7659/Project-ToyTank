/***************************************************************************/
/*!
\file   RenderTargetView.h
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

#ifndef _RENDERTARGETVIEW_H
#define _RENDERTARGETVIEW_H
#pragma once

#include "Texture.h"

class RenderTargetView : public Microsoft::WRL::ComPtr<ID3D11RenderTargetView>
{
public:

    /* core */
    RenderTargetView() = default;
    ~RenderTargetView() = default;
    void Generate(ID3D11Device* device, Texture tex_, const D3D11_RENDER_TARGET_VIEW_DESC* renderTargetDesc);
    void Use(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencil);
    void Unbind(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencil);
    void Clear(ID3D11DeviceContext* context, const DirectX::XMVECTORF32 color);

    Texture GetTextureComPtr() const;
    ID3D11Texture2D* GetTexture() const;
    ID3D11Texture2D** GetAddressOfTexture();
    ID3D11Texture2D* const* GetAddressOfTexture() const;

private:

    /* d3d objects */
    Texture renderTexture_;

};

#endif