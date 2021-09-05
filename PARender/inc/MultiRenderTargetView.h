/***************************************************************************/
/*!
\file   MultiRenderTargetView.h
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

#ifndef _MULTIRENDERTARGETVIEW_H
#define _MULTIRENDERTARGETVIEW_H
#pragma once

#include "RenderTargetView.h"
#include <vector>
#include <DirectXMath.h>

class MultiRenderTargetView
{
public:

    /* core */
    MultiRenderTargetView() = default;
    ~MultiRenderTargetView();
    void Add(RenderTargetView& renderTarget);
    void Use(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencil);
    void Unbind(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencil);
    void Clear(ID3D11DeviceContext* context, DirectX::XMVECTORF32 color);
    std::vector<Texture>* Textures();

private:

    /* d3d objects */
    std::vector<Texture> renderTextures_;
    std::vector<RenderTargetView> renderTargetViews_;
    std::vector<ID3D11RenderTargetView*> renderTargetViewsRaw_;

    /* unsigned */
    unsigned size_ = 0;
};

#endif