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
#include "ShaderResourceView.h"
#include <utility>
#include "MultiShaderResourceView.h"
#include <stdexcept>

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
    add a ShaderResourceView

*/
/****************************************************************************/
void MultiShaderResourceView::Add(ShaderResourceView& srv)
{
    ++size_;

    shaderResourceViewsRaw_.push_back(srv.Get());
    shaderResourceViews_.push_back(srv);
}

/****************************************************************************/
/*!
\brief
    send all ShaderResourceViews

*/
/****************************************************************************/
void MultiShaderResourceView::Use(ID3D11DeviceContext* context)
{
    context->PSSetShaderResources(0, size_, shaderResourceViewsRaw_.data());
}

/****************************************************************************/
/*!
\brief
    unbind all ShaderResourceViews

*/
/****************************************************************************/
void MultiShaderResourceView::Unbind(ID3D11DeviceContext* context)
{
    std::vector<ID3D11ShaderResourceView*> empty;
    empty.resize(size_, nullptr);

    context->PSSetShaderResources(0, size_, empty.data());
}

/****************************************************************************/
/*!
\brief
    set a position in the MultiShaderResourceView

*/
/****************************************************************************/
void MultiShaderResourceView::Set(unsigned index, ShaderResourceView srv)
{
    if (index >= size_)
        throw std::out_of_range("MultiShaderResourceView::Set index out of range");

    shaderResourceViewsRaw_[index] = srv.Get();
    shaderResourceViews_[index] = srv;
}

/****************************************************************************/
/*!
\brief
    reserve a size in the vector of resources

*/
/****************************************************************************/
void MultiShaderResourceView::Resize(unsigned size)
{
    shaderResourceViews_.clear();
    shaderResourceViewsRaw_.clear();
    shaderResourceViews_.resize(size);
    shaderResourceViewsRaw_.resize(size);
    size_ = size;
}

ShaderResourceView MultiShaderResourceView::Get(unsigned i)
{
    return shaderResourceViews_[i];
}
