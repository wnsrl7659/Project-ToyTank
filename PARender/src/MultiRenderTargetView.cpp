/***************************************************************************/
/*!
\file   MultiRenderTargetView.cpp
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

/*============================================================================*\
|| ------------------------------ INCLUDES ---------------------------------- ||
\*============================================================================*/

#include "PARenderPCH.h"
#include "MultiRenderTargetView.h"
#include <utility>

/*===========================================================================*\
|| --------------------------- GLOBAL VARIABLES ---------------------------- ||
\*===========================================================================*/

/*===========================================================================*\
|| -------------------------- STATIC FUNCTIONS ----------------------------- ||
\*===========================================================================*/

/*===========================================================================*\
|| -------------------------- PUBLIC FUNCTIONS ----------------------------- ||
\*===========================================================================*/

/****************************************************************************/
/*!
\brief
    clean up

*/
/****************************************************************************/
MultiRenderTargetView::~MultiRenderTargetView()
{
}

/****************************************************************************/
/*!
\brief
    add a new render target
*/
/****************************************************************************/
void MultiRenderTargetView::Add(RenderTargetView& renderTarget)
{
    ++size_;

    renderTargetViewsRaw_.push_back(renderTarget.Get());
    renderTextures_.push_back(renderTarget.GetTextureComPtr());
    renderTargetViews_.push_back(renderTarget);
}

/****************************************************************************/
/*!
\brief
    Set these Render Target View as the active render target

*/
/****************************************************************************/
void MultiRenderTargetView::Use(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencil)
{
    context->OMSetRenderTargets(size_, renderTargetViewsRaw_.data(), depthStencil);
}

/****************************************************************************/
/*!
\brief
    Set all active render targets as null

*/
/****************************************************************************/
void MultiRenderTargetView::Unbind(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencil)
{
    std::vector<ID3D11RenderTargetView*> empty;
    empty.resize(size_, nullptr);

    context->OMSetRenderTargets(size_, empty.data(), depthStencil);
}

/****************************************************************************/
/*!
\brief
    clear all color

*/
/****************************************************************************/
void MultiRenderTargetView::Clear(ID3D11DeviceContext* context, DirectX::XMVECTORF32 color)
{
    for (unsigned i = 0; i < size_; ++i)
        renderTargetViews_[i].Clear(context, color);
}

std::vector<Texture>* MultiRenderTargetView::Textures()
{
    return &renderTextures_;
}
