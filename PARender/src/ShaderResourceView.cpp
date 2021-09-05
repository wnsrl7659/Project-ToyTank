/***************************************************************************/
/*!
\file   ShaderResourceView.cpp
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

/*============================================================================*\
|| ------------------------------ INCLUDES ---------------------------------- ||
\*============================================================================*/

#include "PARenderPCH.h"
#include "ShaderResourceView.h"
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
    create a ShaderResourceView

*/
/****************************************************************************/
void ShaderResourceView::Generate(ID3D11Device* device, Texture tex_, const D3D11_SHADER_RESOURCE_VIEW_DESC& renderTargetDesc)
{
    shaderTexture_ = tex_;
    HRESULT result = device->CreateShaderResourceView(shaderTexture_.Get(), &renderTargetDesc, ReleaseAndGetAddressOf());
    if (FAILED(result))
    {
        __debugbreak();
    }
}

/****************************************************************************/
/*!
\brief
    send the resource to the shader

*/
/****************************************************************************/
void ShaderResourceView::Use(ID3D11DeviceContext* context)
{
    context->PSSetShaderResources(0, 1, GetAddressOf());
}

/****************************************************************************/
/*!
\brief
    unbind the resource

*/
/****************************************************************************/
void ShaderResourceView::Unbind(ID3D11DeviceContext* context)
{
    std::vector<ID3D11ShaderResourceView*> empty;
    empty.resize(1, nullptr);

    context->PSSetShaderResources(0, 1, empty.data());
}

/****************************************************************************/
/*!
\brief
    Get the whole com ptr of the texture

\return
    comptr wraper class for the texture

*/
/****************************************************************************/
Texture ShaderResourceView::GetTextureComPtr() const
{
    return shaderTexture_;
}

/****************************************************************************/
/*!
\brief
    Get a pointer to the texture

\return
    pointer to the d3d texture

*/
/****************************************************************************/
ID3D11Texture2D* ShaderResourceView::GetTexture() const
{
    return shaderTexture_.Get();
}

/****************************************************************************/
/*!
\brief
    Get a pointer to a pointer to the texture

\return
    pointer to a pointer to the d3d texture

*/
/****************************************************************************/
ID3D11Texture2D** ShaderResourceView::GetAddressOfTexture()
{
    return shaderTexture_.GetAddressOf();
}

/****************************************************************************/
/*!
\brief
    Get a pointer to a pointer to the texture

\return
    pointer to a pointer to the d3d texture

*/
/****************************************************************************/
ID3D11Texture2D* const* ShaderResourceView::GetAddressOfTexture() const
{
    return shaderTexture_.GetAddressOf();
}
