/***************************************************************************/
/*!
\file   Texture.cpp
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

/*============================================================================*\
|| ------------------------------ INCLUDES ---------------------------------- ||
\*============================================================================*/

#include "PARenderPCH.h"
#include "Texture.h"

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
    create a new d3d texture

*/
/****************************************************************************/
void Texture::Generate(ID3D11Device* device, D3D11_TEXTURE2D_DESC& desc, D3D11_SUBRESOURCE_DATA* data)
{
    HRESULT hr = device->CreateTexture2D(&desc, data, GetAddressOf());
    if (FAILED(hr))
    {
        __debugbreak();
    }
}

/****************************************************************************/
/*!
\brief
    create a new d3d texture

*/
/****************************************************************************/
void Texture::Generate(ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT format, UINT bindFlags, DXGI_SAMPLE_DESC sampleDesc,
    D3D11_USAGE usage, UINT cpuAccessFlags, UINT miscFlags, D3D11_SUBRESOURCE_DATA* data, UINT mipLevels, UINT arraySize)
{
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = mipLevels;
    textureDesc.ArraySize = arraySize;
    textureDesc.Format = format;
    textureDesc.SampleDesc = sampleDesc;
    textureDesc.Usage = usage;
    textureDesc.BindFlags = bindFlags;
    textureDesc.CPUAccessFlags = cpuAccessFlags;
    textureDesc.MiscFlags = miscFlags;

    HRESULT hr = device->CreateTexture2D(&textureDesc, data, GetAddressOf());
    if (FAILED(hr))
    {
        __debugbreak();
    }
}


/*===========================================================================*\
|| -------------------------- PRIVATE FUNCTIONS ---------------------------- ||
\*===========================================================================*/