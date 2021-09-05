/***************************************************************************/
/*!
\file   MultiShaderResourceView.h
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

#ifndef _MULTISHADERRESOURCEVIEW_H
#define _MULTISHADERRESOURCEVIEW_H
#pragma once

#include "ShaderResourceView.h"
#include <vector>

class MultiShaderResourceView
{
public:

    /* core */
    MultiShaderResourceView() = default;
    ~MultiShaderResourceView() = default;

    void Add(ShaderResourceView& srv);
    void Use(ID3D11DeviceContext* context);
    void Unbind(ID3D11DeviceContext* context);
    void Set(unsigned index, ShaderResourceView srv);
    void Resize(unsigned size);
    ShaderResourceView Get(unsigned i);
private:

    /* d3d objects */
    std::vector<ShaderResourceView> shaderResourceViews_;
    std::vector<ID3D11ShaderResourceView*> shaderResourceViewsRaw_;

    /* unsigned */
    unsigned size_ = 0;
};

#endif