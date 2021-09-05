/***************************************************************************/
/*!
\file   DepthStencilView.cpp
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

/*============================================================================*\
|| ------------------------------ INCLUDES ---------------------------------- ||
\*============================================================================*/

#include "PARenderPCH.h"
#include "DepthStencilView.h"

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
    Create a new DepthStencilView

\param device
    logical d3d device

\param buffer
    texture to hold depth info

\param desc
    description of the depth stencil view

*/
/****************************************************************************/
void DepthStencilView::Generate(ID3D11Device* device, const Texture buffer, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc)
{
    depthBuffer_ = buffer;

    HRESULT hr = device->CreateDepthStencilView(depthBuffer_.Get(), &desc, ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        __debugbreak();
    }
}

/****************************************************************************/
/*!
\brief
    clear the depthstencil buffer

\param context
    d3d context

\param depth
    what depth the fill with

\param stencil
    what stencil to fill with

*/
/****************************************************************************/
void DepthStencilView::Clear(ID3D11DeviceContext* context, const FLOAT depth, const UINT8 stencil)
{
    context->ClearDepthStencilView(Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
}

/****************************************************************************/
/*!
\brief
    clear the depth buffer

\param context
    d3d context

\param depth
    what depth the fill with

*/
/****************************************************************************/
void DepthStencilView::ClearDepth(ID3D11DeviceContext* context, const FLOAT depth)
{
    context->ClearDepthStencilView(Get(), D3D11_CLEAR_DEPTH, depth, 0xFF);
}

/****************************************************************************/
/*!
\brief
    clear the stencil buffer

\param context
    d3d context

\param stencil
    what stencil to fill with

*/
/****************************************************************************/
void DepthStencilView::ClearStencil(ID3D11DeviceContext* context, const UINT8 stencil)
{
    context->ClearDepthStencilView(Get(), D3D11_CLEAR_STENCIL, 1.0, stencil);
}

/****************************************************************************/
/*!
\brief
    Get the whole com ptr of the texture

\return
    comptr wraper class for the texture

*/
/****************************************************************************/
Texture DepthStencilView::GetTextureComPtr() const
{
    return depthBuffer_;
}

/****************************************************************************/
/*!
\brief
    Get a pointer to the raw depth buffer
\return
    pointer to the raw d3d texture

*/
/****************************************************************************/
ID3D11Texture2D* DepthStencilView::GetTexture() const
{
    return depthBuffer_.Get();
}

/****************************************************************************/
/*!
\brief
    Get a pointer to a pointer to the raw depth buffer

\return
    pointer to a pointer to the raw d3d texture

*/
/****************************************************************************/
ID3D11Texture2D** DepthStencilView::GetAddressOfTexture()
{
    return depthBuffer_.GetAddressOf();
}

/****************************************************************************/
/*!
\brief
    Get a pointer to a pointer to the raw depth buffer

\return
    pointer to a pointer to the raw d3d texture

*/
/****************************************************************************/
ID3D11Texture2D* const* DepthStencilView::GetAddressOfTexture() const
{
    return depthBuffer_.GetAddressOf();
}
