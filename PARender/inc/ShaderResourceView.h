/***************************************************************************/
/*!
\file   ShaderResourceView.h
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

#ifndef _SHADERRESOURCEVIEW_H
#define _SHADERRESOURCEVIEW_H
#pragma once

#include "Texture.h"

class ShaderResourceView : public Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>
{
public:

    /* core */
    ShaderResourceView() = default;
    ~ShaderResourceView() = default;

    void Generate(ID3D11Device* device, Texture tex_, const D3D11_SHADER_RESOURCE_VIEW_DESC& renderTargetDesc);
    void Use(ID3D11DeviceContext* context);
    void Unbind(ID3D11DeviceContext* context);

    Texture GetTextureComPtr() const;
    ID3D11Texture2D* GetTexture() const;
    ID3D11Texture2D** GetAddressOfTexture();
    ID3D11Texture2D* const* GetAddressOfTexture() const;

private:

    /* d3d objects */
    Texture shaderTexture_;

};

#endif