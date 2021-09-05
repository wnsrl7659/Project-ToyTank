/***************************************************************************/
/*!
\file   RenderTargetView.cpp
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

/*============================================================================*\
|| ------------------------------ INCLUDES ---------------------------------- ||
\*============================================================================*/

#include "PARenderPCH.h"
#include "RenderTargetView.h"

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
    create a render target view

*/
/****************************************************************************/
void RenderTargetView::Generate(ID3D11Device* device, Texture tex_, const D3D11_RENDER_TARGET_VIEW_DESC* renderTargetDesc)
{
    renderTexture_ = tex_;

    HRESULT hr = device->CreateRenderTargetView(renderTexture_.Get(), renderTargetDesc, ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        __debugbreak();
    }
}

/****************************************************************************/
/*!
\brief
    Set this Render Target View as the active render target

*/
/****************************************************************************/
void RenderTargetView::Use(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencil)
{
    context->OMSetRenderTargets(1, GetAddressOf(), depthStencil);
}

/****************************************************************************/
/*!
\brief
    unbind the render target

*/
/****************************************************************************/
void RenderTargetView::Unbind(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencil)
{
    std::vector<ID3D11RenderTargetView*> empty = { nullptr };
    context->OMSetRenderTargets(1, empty.data(), depthStencil);
}

/****************************************************************************/
/*!
\brief
    Clear the render target with a color

*/
/****************************************************************************/
void RenderTargetView::Clear(ID3D11DeviceContext* context, const DirectX::XMVECTORF32 color)
{
    context->ClearRenderTargetView(Get(), color);
}

/****************************************************************************/
/*!
\brief
    Get the whole com ptr of the texture

\return
    comptr wraper class for the texture

*/
/****************************************************************************/
Texture RenderTargetView::GetTextureComPtr() const
{
    return renderTexture_;
}

/****************************************************************************/
/*!
\brief
    Get a pointer to a pointer to the raw render texture
\return
   pointer to a pointer to  the raw d3d texture

*/
/****************************************************************************/
ID3D11Texture2D** RenderTargetView::GetAddressOfTexture()
{
    return renderTexture_.GetAddressOf();
}

/****************************************************************************/
/*!
\brief
    Get a pointer to a pointer to the raw render texture
\return
   pointer to a pointer to  the raw d3d texture

*/
/****************************************************************************/
ID3D11Texture2D* const* RenderTargetView::GetAddressOfTexture() const
{
    return renderTexture_.GetAddressOf();
}

/****************************************************************************/
/*!
\brief
    Get a pointer to the raw render texture
\return
    pointer to the raw d3d texture

*/
/****************************************************************************/
ID3D11Texture2D* RenderTargetView::GetTexture() const
{
    return renderTexture_.Get();
}


