#include "PARenderPCH.h"

#include "RenderEngine.h"
#include "Renderer.h"
#include "Scene.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "../inc/DXTK/WICTextureLoader.h"
#include <random>

// Use NVIDIA on laptops
extern "C" {
    _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
}

//comPtr
using namespace Microsoft::WRL;

//static instance initialization
Rendering::RenderEngine::IEngine* Rendering::RenderEngine::mInstance = nullptr;

//TODO(SETH) Implement window class and move this there
//APP matrix for window width/height.
DirectX::XMMATRIX appProjection;

static void UpdateAppProjectionMat();

static float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

namespace Rendering {
    static void CalculateNormals(std::vector<Vertex>* vertices, std::vector<unsigned>* indicies)
    {
        unsigned iSize = unsigned(indicies->size());
        for (unsigned i = 0; i < iSize; i += 3)
        {
            // face normals
            unsigned i0 = i + 2;
            unsigned i1 = i + 1;
            i0 = (*indicies)[i0];
            i1 = (*indicies)[i1];
            unsigned i2 = (*indicies)[i];

            DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&(*vertices)[i0].position);
            DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&(*vertices)[i1].position);
            DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&(*vertices)[i2].position);
            DirectX::XMFLOAT3 P;
            DirectX::XMStoreFloat3(&P, DirectX::XMVector3Normalize(
                DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(C, A))));

            // vertex normal
            (*vertices)[i0].normal.x += P.x;
            (*vertices)[i0].normal.y += P.y;
            (*vertices)[i0].normal.z += P.z;

            (*vertices)[i1].normal.x += P.x;
            (*vertices)[i1].normal.y += P.y;
            (*vertices)[i1].normal.z += P.z;

            (*vertices)[i2].normal.x += P.x;
            (*vertices)[i2].normal.y += P.y;
            (*vertices)[i2].normal.z += P.z;
        }

        unsigned vSize = unsigned(vertices->size());
        for (unsigned i = 0; i < vSize; ++i)
        {
            DirectX::XMStoreFloat3(&(*vertices)[i].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&(*vertices)[i].normal)));
        }
    }

    static void CalculateTangents(std::vector<Vertex>* vertices, std::vector<unsigned>* indicies)
    {
        size_t triangleCount = indicies->size();
        size_t vertexCount = vertices->size();

        for (size_t i = 0; i < triangleCount; i += 3)
        {
            unsigned index0 = (*indicies)[i];
            unsigned index1 = (*indicies)[i + 1];
            unsigned index2 = (*indicies)[i + 2];

            const Vertex& v1 = (*vertices)[index0];
            const Vertex& v2 = (*vertices)[index1];
            const Vertex& v3 = (*vertices)[index2];

            float x1 = v2.position.x - v1.position.x;
            float x2 = v3.position.x - v1.position.x;
            float y1 = v2.position.y - v1.position.y;
            float y2 = v3.position.y - v1.position.y;
            float z1 = v2.position.z - v1.position.z;
            float z2 = v3.position.z - v1.position.z;

            float s1 = v2.texCoords.x - v1.texCoords.x;
            float s2 = v3.texCoords.x - v1.texCoords.x;
            float t1 = v2.texCoords.y - v1.texCoords.y;
            float t2 = v3.texCoords.y - v1.texCoords.y;

            float r = 1.0F / std::max((s1 * t2 - s2 * t1), 0.001f);

            (*vertices)[index0].tangent.x += (t2 * x1 - t1 * x2) * r;
            (*vertices)[index0].tangent.y += (t2 * y1 - t1 * y2) * r;
            (*vertices)[index0].tangent.z += (t2 * z1 - t1 * z2) * r;

            (*vertices)[index1].tangent.x += (t2 * x1 - t1 * x2) * r;
            (*vertices)[index1].tangent.y += (t2 * y1 - t1 * y2) * r;
            (*vertices)[index1].tangent.z += (t2 * z1 - t1 * z2) * r;

            (*vertices)[index2].tangent.x += (t2 * x1 - t1 * x2) * r;
            (*vertices)[index2].tangent.y += (t2 * y1 - t1 * y2) * r;
            (*vertices)[index2].tangent.z += (t2 * z1 - t1 * z2) * r;

            (*vertices)[index0].bitangent.x += (s1 * x2 - s2 * x1) * r;
            (*vertices)[index0].bitangent.y += (t2 * y1 - t1 * y2) * r;
            (*vertices)[index0].bitangent.z += (s1 * z2 - s2 * z1) * r;

            (*vertices)[index1].bitangent.x += (s1 * x2 - s2 * x1) * r;
            (*vertices)[index1].bitangent.y += (s1 * y2 - s2 * y1) * r;
            (*vertices)[index1].bitangent.z += (s1 * z2 - s2 * z1) * r;

            (*vertices)[index2].bitangent.x += (s1 * x2 - s2 * x1) * r;
            (*vertices)[index2].bitangent.y += (s1 * y2 - s2 * y1) * r;
            (*vertices)[index2].bitangent.z += (s1 * z2 - s2 * z1) * r;
        }

        for (size_t i = 0; i < vertexCount; ++i)
        {
            DirectX::XMVECTOR tangent = DirectX::XMLoadFloat3(&(*vertices)[i].tangent);
            DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(&(*vertices)[i].normal);
            DirectX::XMVECTOR bitangent = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&(*vertices)[i].bitangent));

            // orthonormalize
            DirectX::XMStoreFloat3(&(*vertices)[i].tangent, DirectX::XMVector3Normalize(
                DirectX::XMVectorSubtract(tangent, DirectX::XMVector3Dot(tangent, DirectX::XMLoadFloat3(&(*vertices)[i].normal)))));

            // headedness
            DirectX::XMFLOAT3 dotp;
            DirectX::XMStoreFloat3(&dotp, DirectX::XMVector3Dot(DirectX::XMVector3Cross(tangent, bitangent), normal));
            // int headedness = dotp.x > 0 ? 1 : -1;
            // (*vertices)[i].tangent.x /= headedness;
           //  (*vertices)[i].tangent.y /= headedness;
            // (*vertices)[i].tangent.z /= headedness;

             // bitangent
            DirectX::XMStoreFloat3(&(*vertices)[i].bitangent, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&(*vertices)[i].tangent), normal));
        }
    }

    IEngine* RenderEngine::GetInstance() {
        if (mInstance == nullptr)
        {
            mInstance = new RenderEngine();
        }

        return static_cast<IEngine*>(mInstance);
    }

    void RenderEngine::Initialize(PARenderInfo renderinfo)
    {
        //construct the window

        //TODO(Seth) Query and set MSAA state elsewhere
        DXGI_SAMPLE_DESC msaaSampleDesc{};
        msaaSampleDesc.Count = 1;
        msaaSampleDesc.Quality = 0;

        //TODO(Seth) Not needed if we're excluding texture loading with WIC and DXTK
        //COM CoInit
        //__Win32COMCoInitializeWIC();
        Utility::COM::__Win32COMCoInitializeWIC();

        //TODO(Seth) Document
        //TODO(Seth) Error handling utility functions for VSOutput and Popup windows

        //create Resources//

        //create device and swapchain
        DXGI_MODE_DESC bufferDesc{};
        bufferDesc.Width = Utility::GetWindowWidth();
        bufferDesc.Height = Utility::GetWindowHeight();
        bufferDesc.RefreshRate.Numerator = 0;
        bufferDesc.RefreshRate.Denominator = 1;
        bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        DXGI_SWAP_CHAIN_DESC swapDesc{};
        swapDesc.BufferDesc = bufferDesc;
        swapDesc.SampleDesc = msaaSampleDesc;
        swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapDesc.BufferCount = 1;
        swapDesc.OutputWindow = Utility::GetWindowHandle();
        swapDesc.Windowed = true;
        //TODO(Seth) swapeffect port to swap_effect_flip_discard
        swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        UINT deviceFlags = 0;
#if _DEBUG
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0 };

        D3D_FEATURE_LEVEL featureLevel{};

        //TODO(Seth) Deferred Context
        HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &featureLevels[0], _countof(featureLevels),
            D3D11_SDK_VERSION, &swapDesc, mSwapChain.GetAddressOf(), mDevice.ReleaseAndGetAddressOf(), &featureLevel, mContext.ReleaseAndGetAddressOf());

        if (hr == 0x887a002d)
        {
            deviceFlags = 0;
            hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &featureLevels[0], _countof(featureLevels),
                D3D11_SDK_VERSION, &swapDesc, mSwapChain.GetAddressOf(), mDevice.ReleaseAndGetAddressOf(), &featureLevel, mContext.ReleaseAndGetAddressOf());
        }

        if (hr == E_INVALIDARG || hr == DXGI_ERROR_SDK_COMPONENT_MISSING)
        {
            hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &featureLevels[1], _countof(featureLevels) - 1,
                D3D11_SDK_VERSION, &swapDesc, mSwapChain.GetAddressOf(), mDevice.ReleaseAndGetAddressOf(), &featureLevel, mContext.ReleaseAndGetAddressOf());
        }
        if (FAILED(hr))
        {
            __debugbreak();
        }

        //initialize debug device
#if _DEBUG
        if (deviceFlags & D3D11_CREATE_DEVICE_DEBUG)
        {
            hr = mDevice.As(&mDebugDevice);
            if (FAILED(hr))
            {
                __debugbreak();
            }
        }
#endif

        //Get SwapChain Buffer
        hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(mSwapChainBuffer.GetAddressOf()));

        hr = DisableAltEnter(true);
        if (FAILED(hr))
        {
            __debugbreak();
        }

        //create render targets
        Texture pBackBuffer;
        hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(pBackBuffer.GetAddressOf()));
        if (FAILED(hr))
        {
            __debugbreak();
        }


        D3D11_BLEND_DESC omDesc;
        ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));

        for (unsigned i = 0; i < 6; ++i)
        {
            omDesc.RenderTarget[i].BlendEnable = TRUE;
            omDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            omDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            omDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
            omDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
            omDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
            omDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            omDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        }


        if (FAILED(mDevice->CreateBlendState(&omDesc, &mBlendState)))
        {
            __debugbreak();
        }

        omDesc.RenderTarget[0].BlendEnable = FALSE;
        omDesc.RenderTarget[1].BlendEnable = FALSE;
        if (FAILED(mDevice->CreateBlendState(&omDesc, &mBlendStateOff)))
        {
            __debugbreak();
        }
        //hr = mDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, mView.GetAddressOf());
        //if (FAILED(hr))
        //{
        //    __debugbreak();
        //}

        //D3D11_TEXTURE2D_DESC postDesc;
        //pBackBuffer.Get()->GetDesc(&postDesc);
        //mPostProcessBuffer.Generate(mDevice.Get(), postDesc);

        mView.Generate(mDevice.Get(), pBackBuffer, nullptr);

        D3D11_SHADER_RESOURCE_VIEW_DESC postTextDesc;
        ZeroMemory(&postTextDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        postTextDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        postTextDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        postTextDesc.Texture2D.MipLevels = 1;

        D3D11_RENDER_TARGET_VIEW_DESC rtvDescP;
        ZeroMemory(&rtvDescP, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
        rtvDescP.Format = postTextDesc.Format;
        rtvDescP.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        mPostProcessBuffer.Generate(mDevice.Get(), bufferDesc.Width, bufferDesc.Height, postTextDesc.Format,
            D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, msaaSampleDesc);
        RenderTargetView rt;
        rt.Generate(mDevice.Get(), mPostProcessBuffer, &rtvDescP);
        mPostProcessView.Add(rt);
        ShaderResourceView sr;
        sr.Generate(mDevice.Get(), mPostProcessBuffer, postTextDesc);
        mPostProcessTexture.Add(sr);
        mFinalTexture.Add(sr);

        mBloomBuffer.Generate(mDevice.Get(), bufferDesc.Width, bufferDesc.Height, postTextDesc.Format,
            D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, msaaSampleDesc);
        RenderTargetView rt2;
        rt2.Generate(mDevice.Get(), mBloomBuffer, &rtvDescP);
        mPostProcessView.Add(rt2);
        ShaderResourceView sr2;
        sr2.Generate(mDevice.Get(), mBloomBuffer, postTextDesc);
        mPostProcessTexture.Add(sr2);


        //create depth stencil buffer
        //D3D11_TEXTURE2D_DESC depthDesc;
        //depthDesc.Width = bufferDesc.Width;
        //depthDesc.Height = bufferDesc.Height;
        //depthDesc.MipLevels = 1;
        //depthDesc.ArraySize = 1;
        //depthDesc.Format = DXGI_FORMAT_D32_FLOAT;  //pDeviceSettings->d3d11.AutoDepthStencilFormat;
        //depthDesc.SampleDesc = msaaSampleDesc;
        //depthDesc.Usage = D3D11_USAGE_DEFAULT;
        //depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        //depthDesc.CPUAccessFlags = 0;
        //depthDesc.MiscFlags = 0;

        //hr = mDevice->CreateTexture2D(&depthDesc, nullptr, mDepthStencilBuffer.GetAddressOf());
        //if (FAILED(hr))
        //{
        //    __debugbreak();
        //}

        mDepthStencilBuffer.Generate(mDevice.Get(), bufferDesc.Width, bufferDesc.Height,
            DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, msaaSampleDesc);
        mMiniDepthStencilBuffer.Generate(mDevice.Get(), bufferDesc.Width, bufferDesc.Height,
            DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, msaaSampleDesc);

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        mDepthView.Generate(mDevice.Get(), mDepthStencilBuffer, depthStencilViewDesc);
        mMiniDepthView.Generate(mDevice.Get(), mMiniDepthStencilBuffer, depthStencilViewDesc);
        /*hr = mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &depthStencilViewDesc, mDepthView.GetAddressOf());
        if (FAILED(hr))
        {
            __debugbreak();
        }*/

        //create state objects//
        D3D11_SAMPLER_DESC samplerDesc{};
        ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = mDevice->CreateSamplerState(&samplerDesc, mSamplerState[0].ReleaseAndGetAddressOf());
        if (FAILED(hr))
        {
            __debugbreak();
        }

        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = mDevice->CreateSamplerState(&samplerDesc, mSamplerState[1].ReleaseAndGetAddressOf());
        if (FAILED(hr))
        {
            __debugbreak();
        }

        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = mDevice->CreateSamplerState(&samplerDesc, mSamplerState[2].ReleaseAndGetAddressOf());
        if (FAILED(hr))
        {
            __debugbreak();
        }

        //create depth-stencil state
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        //depth test params
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        //stencil test params
        depthStencilDesc.StencilEnable = true;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;
        //stencil operations if pixel is front-facing
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        //stencil operations if pixel is back-facing
        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        hr = mDevice->CreateDepthStencilState(&depthStencilDesc, mDepthStencilState.GetAddressOf());
        if (FAILED(hr))
        {
            __debugbreak();
        }

        //create depth-stencil state
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        //depth test params
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        //stencil test params
        depthStencilDesc.StencilEnable = false;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;
        //stencil operations if pixel is front-facing
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        //stencil operations if pixel is back-facing
        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        hr = mDevice->CreateDepthStencilState(&depthStencilDesc, mDepthStencilStateSkybox.GetAddressOf());
        if (FAILED(hr))
        {
            __debugbreak();
        }

        //TODO(Seth) Initialize Rasterizer
        D3D11_RASTERIZER_DESC rasterDesc{};
        ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterDesc.AntialiasedLineEnable = FALSE;
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.0f;
        rasterDesc.DepthClipEnable = TRUE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = TRUE;
        rasterDesc.MultisampleEnable = FALSE;
        rasterDesc.ScissorEnable = FALSE;
        rasterDesc.SlopeScaledDepthBias = 0.0f;

        hr = mDevice->CreateRasterizerState(&rasterDesc, mRasterizerState.GetAddressOf());
        if (FAILED(hr))
        {
            __debugbreak();
        }


#pragma region TempRegion

        // Create the constant buffers for the variables defined in the vertex shader.
        D3D11_BUFFER_DESC constantBufferDesc;
        ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

        constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
        constantBufferDesc.CPUAccessFlags = 0;
        constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        //TODO(Seth) Should be owned by the window class and handle resizing in callback of some form
        //mContext->UpdateSubresource(mConstantBuffers[CB_Application], 0, nullptr, &projectionMatrix, 0, 0);
        mContext->PSSetSamplers(0, 1, mSamplerState[0].GetAddressOf());

        //TODO(SETH) THIS SHOULD BE INTERNAL BUT ELSEWHERE
        mRenderer.reset(new Renderer());

        //UpdateAppProjectionMat();
       // mRenderer->SetRendererConstantBufferData(&appProjection, Renderer::ConstantBuffer::CB_Application);

#pragma endregion

        //mAssetLoader.reset(new Internal::AssetLoader(mDevice.Get()));

        //return S_OK;

        // Generate GBuffer
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
        ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
        rtvDesc.Format = srvDesc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        for (unsigned i = 0; i < 6; ++i)
        {
            Texture gBufferTex;
            gBufferTex.Generate(mDevice.Get(), bufferDesc.Width, bufferDesc.Height, srvDesc.Format,
                D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, msaaSampleDesc);

            RenderTargetView rt;
            rt.Generate(mDevice.Get(), gBufferTex, &rtvDesc);
            mGBuffer.Add(rt);

            ShaderResourceView sr;
            sr.Generate(mDevice.Get(), gBufferTex, srvDesc);
            mGBufferTextures.Add(sr);
        }

        ShaderResourceView skybox = mRenderer->GetSkybox();

        /* SSAO */
        mSSAOKernal.reserve(16);
        std::mt19937 gen;
        std::uniform_real_distribution<float> dis(-1.f, 1.f);
        for (unsigned int i = 0; i < 16; ++i)
        {
            DirectX::XMVECTOR sample = { dis(gen) * 2.0f - 1.0f, dis(gen) * 2.0f - 1.0f, dis(gen), 1 };
            sample = DirectX::XMVector3Normalize(sample);
            sample = DirectX::XMVectorScale(sample, dis(gen));
            float scale = float(i) / 64.0f;
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample = DirectX::XMVectorScale(sample, scale);
            DirectX::XMFLOAT4 sample4;
            DirectX::XMStoreFloat4(&sample4, sample);
            mSSAOKernal.push_back(sample4);
        }

        mSSAONoiseTexture = GetTexture(L"../PAEngine/data/textures/SSAONoise.jpg");

        Texture SSAOOutTexture;
        SSAOOutTexture.Generate(mDevice.Get(), bufferDesc.Width, bufferDesc.Height, srvDesc.Format,
            D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, msaaSampleDesc);
        mSSAOOutputTexture.Generate(mDevice.Get(), SSAOOutTexture, srvDesc);
        mSSAOView.Generate(mDevice.Get(), SSAOOutTexture, &rtvDesc);

        // ssao blur
        Texture SSAOOutBlurBuffer;
        SSAOOutBlurBuffer.Generate(mDevice.Get(), bufferDesc.Width, bufferDesc.Height, srvDesc.Format,
            D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, msaaSampleDesc);
        mSSAOOutputBlurTexture.Generate(mDevice.Get(), SSAOOutBlurBuffer, srvDesc);
        mSSAOBlurView.Generate(mDevice.Get(), SSAOOutBlurBuffer, &rtvDesc);

        //bloom blur
        mBloomTexture.Generate(mDevice.Get(), mBloomBuffer, srvDesc);
        mBloomBlurBuffer.Generate(mDevice.Get(), bufferDesc.Width, bufferDesc.Height, srvDesc.Format,
            D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, msaaSampleDesc);
        mBloomBlurView.Generate(mDevice.Get(), mBloomBlurBuffer, &rtvDesc);
        mBloomBlurTexture.Generate(mDevice.Get(), mBloomBlurBuffer, srvDesc);

        // bloom staging buffer
        mBloomBlurStageBuffer.Generate(mDevice.Get(), bufferDesc.Width, bufferDesc.Height, srvDesc.Format,
            D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, msaaSampleDesc);
        mBloomBlurStageView.Generate(mDevice.Get(), mBloomBlurStageBuffer, &rtvDesc);
        mBloomBlurStageTexture.Generate(mDevice.Get(), mBloomBlurStageBuffer, srvDesc);

        mFinalTexture.Add(mBloomBlurTexture);

        // vignette

        mVignetteBuffer.Generate(mDevice.Get(), bufferDesc.Width, bufferDesc.Height, srvDesc.Format,
            D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, msaaSampleDesc);
        mVignetteTexture.Generate(mDevice.Get(), mVignetteBuffer, srvDesc);
        mVignette.Generate(mDevice.Get(), mVignetteBuffer, &rtvDesc);
        mFinalTexture.Add(mVignetteTexture);

        mRenderer->SetSSAOInfo(mSSAOKernal);

        /* shadow buffer */

        D3D11_FEATURE_DATA_D3D9_SHADOW_SUPPORT isD3D9ShadowSupported;
        ZeroMemory(&isD3D9ShadowSupported, sizeof(isD3D9ShadowSupported));
        mDevice->CheckFeatureSupport(D3D11_FEATURE_D3D9_SHADOW_SUPPORT, &isD3D9ShadowSupported,
            sizeof(D3D11_FEATURE_D3D9_SHADOW_SUPPORT));

        if (!isD3D9ShadowSupported.SupportsDepthAsTextureWithLessEqualComparisonFilter)
        {
            __debugbreak();
        }

        mShadowBuffer.Generate(mDevice.Get(), 4096, 4096,
            DXGI_FORMAT_R24G8_TYPELESS, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL, msaaSampleDesc);

        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        mShadowTexture.Generate(mDevice.Get(), mShadowBuffer, srvDesc);

        ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        mShadowView.Generate(mDevice.Get(), mShadowBuffer, depthStencilViewDesc);

        ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterDesc.AntialiasedLineEnable = FALSE;
        rasterDesc.CullMode = D3D11_CULL_FRONT;
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.0f;
        rasterDesc.DepthClipEnable = TRUE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = TRUE;
        rasterDesc.MultisampleEnable = FALSE;
        rasterDesc.ScissorEnable = FALSE;
        rasterDesc.SlopeScaledDepthBias = 0.0f;

        hr = mDevice->CreateRasterizerState(&rasterDesc, mShadowRasterizerState.GetAddressOf());
        if (FAILED(hr))
        {
            __debugbreak();
        }

        ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterDesc.AntialiasedLineEnable = TRUE;
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.0f;
        rasterDesc.DepthClipEnable = TRUE;
        rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
        rasterDesc.FrontCounterClockwise = TRUE;
        rasterDesc.MultisampleEnable = FALSE;
        rasterDesc.ScissorEnable = FALSE;
        rasterDesc.SlopeScaledDepthBias = 0.0f;

        hr = mDevice->CreateRasterizerState(&rasterDesc, mOutlineRasterizerState.GetAddressOf());
        if (FAILED(hr))
        {
            __debugbreak();
        }

        //mShadowSamplerState
        D3D11_SAMPLER_DESC comparisonSamplerDesc;
        ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
        comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        comparisonSamplerDesc.BorderColor[0] = 1.0f;
        comparisonSamplerDesc.BorderColor[1] = 1.0f;
        comparisonSamplerDesc.BorderColor[2] = 1.0f;
        comparisonSamplerDesc.BorderColor[3] = 1.0f;
        comparisonSamplerDesc.MinLOD = 0.f;
        comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        comparisonSamplerDesc.MipLODBias = 0.f;
        comparisonSamplerDesc.MaxAnisotropy = 0;
        comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
        comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

        hr = mDevice->CreateSamplerState(&comparisonSamplerDesc, &mShadowSamplerState);
        if (FAILED(hr))
        {
            __debugbreak();
        }
#if _DEBUG
        mShadowShader = reinterpret_cast<ShaderProgram*>(CreateShaderProgram("..\\PARender\\bin\\Debug\\ShadowMapVertex.cso",
            "..\\PARender\\bin\\Debug\\ShadowMapPixel.cso"));
#else
        mShadowShader = reinterpret_cast<ShaderProgram*>(CreateShaderProgram("..\\PARender\\bin\\Release\\ShadowMapVertex.cso",
            "..\\PARender\\bin\\Release\\ShadowMapPixel.cso"));
#endif

#if _DEBUG
        mOutlineShader = reinterpret_cast<ShaderProgram*>(CreateShaderProgram("..\\PARender\\bin\\Debug\\OutlineVertex.cso",
            "..\\PARender\\bin\\Debug\\OutlinePixel.cso"));
#else
        mOutlineShader = reinterpret_cast<ShaderProgram*>(CreateShaderProgram("..\\PARender\\bin\\Release\\OutlineVertex.cso",
            "..\\PARender\\bin\\Release\\OutlinePixel.cso"));
#endif
        mOutlineTexture = GetTexture(L"../PAEngine/data/textures/Outline.png");

        /* attach gbuffer */
        mGBufferTextures.Add(mSSAOOutputBlurTexture);
        mGBufferTextures.Add(mSSAONoiseTexture);
        mGBufferTextures.Add(skybox);
        mGBufferTextures.Add(mShadowTexture);
        mGBufferTextures.Add(mOutlineTexture);

        ShaderResourceView srG = mGBufferTextures.Get(2);
        mSSAOTextures.Add(mSSAONoiseTexture);
        mSSAOTextures.Add(srG);
        srG = mGBufferTextures.Get(5);
        mSSAOTextures.Add(srG);
#if _DEBUG

#else
        mSwapChain->SetFullscreenState(true, NULL);
#endif // DEBUG


    }

    void RenderEngine::StartFrame(float dt)
    {
        //bind resources to pipeline
        ID3D11DepthStencilView* depthTargetView = mDepthView.Get();
        DirectX::XMVECTORF32 color = { { { 0, 0, 0, 1.f } } }; //{ { { 0.631372549f, 0.847058824f, 1.f, 1.f } } };
        mView.Clear(mContext.Get(), color);
        mGBuffer.Clear(mContext.Get(), color);
        mDepthView.Clear(mContext.Get(), 1.f, 0xFF);
        mMiniDepthView.Clear(mContext.Get(), 1.f, 0xFF);
        mShadowView.Clear(mContext.Get(), 1.f, 0xFF);
        mSSAOView.Clear(mContext.Get(), { { { 1.f, 1.f,1.f, 1.f } } });
        mSSAOBlurView.Clear(mContext.Get(), { { { 1.f, 1.f,1.f, 1.f } } });
        mBloomBlurView.Clear(mContext.Get(), { { { 1.f, 1.f,1.f, 1.f } } });
        mBloomBlurStageView.Clear(mContext.Get(), { { { 1.f, 1.f,1.f, 1.f } } });
        mVignette.Clear(mContext.Get(), { { { 1.f, 1.f,1.f, 1.f } } });

        //gbuffer
        mContext->OMSetDepthStencilState(mDepthStencilState.Get(), 1);
        mContext->PSSetSamplers(0, 1, mSamplerState[0].GetAddressOf());
        mContext->RSSetState(mRasterizerState.Get());
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        UINT sampleMask = 0xffffffff;
        mContext->OMSetBlendState(mBlendStateOff.Get(), blendFactor, sampleMask);
    }

    void RenderEngine::RenderScene(float dt)
    {
        ID3D11DepthStencilView* depthTargetView = mDepthView.Get();
        ID3D11DepthStencilView* depthTargetViewMin = mMiniDepthView.Get();
        IScene* scene = reinterpret_cast<IScene*>(mScenes.front().get());
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        UINT sampleMask = 0xffffffff;

        // fill gbuffer
        mGBuffer.Use(mContext.Get(), depthTargetView);
        mContext->OMSetBlendState(mBlendState.Get(), blendFactor, sampleMask);
        mRenderer->Render(scene);
        mContext->OMSetBlendState(mBlendStateOff.Get(), blendFactor, sampleMask);
        mContext->RSSetState(mOutlineRasterizerState.Get());
        mGBufferTextures.Use(mContext.Get());
        mRenderer->RenderOutline(scene, mOutlineShader);
        mContext->RSSetState(mRasterizerState.Get());
        mGBuffer.Unbind(mContext.Get(), depthTargetView);

        // mContext->CopyResource(reinterpret_cast<ID3D11Resource*>(depthTargetView), reinterpret_cast<ID3D11Resource*>(depthTargetViewMin));

      //   mMiniDepthView.Clear(mContext.Get(), 1.f, 0xFF);
       //  depthTargetViewMin = mMiniDepthView.Get();

         // Shadow Map
        mContext->RSSetState(mShadowRasterizerState.Get());
        mContext->PSSetSamplers(1, 1, mShadowSamplerState.GetAddressOf());
        mContext->OMSetRenderTargets(0, nullptr, mShadowView.Get());
        mRenderer->RenderShadowMap(scene, mShadowShader);
        mContext->RSSetState(mRasterizerState.Get());

        mContext->PSSetSamplers(0, 1, mSamplerState[2].GetAddressOf());
        mContext->OMSetDepthStencilState(mDepthStencilStateSkybox.Get(), 1);

        // SSAO
        mGBufferTextures.Unbind(mContext.Get());
        mSSAOView.Use(mContext.Get(), depthTargetView);
        mSSAOTextures.Use(mContext.Get());
        mRenderer->RenderSSAO(scene, mSSAOKernal);
        mSSAOTextures.Unbind(mContext.Get());
        mSSAOView.Unbind(mContext.Get(), depthTargetView);

        // SSAO Blur
        mSSAOBlurView.Use(mContext.Get(), depthTargetView);
        mSSAOOutputTexture.Use(mContext.Get());
        mRenderer->BlurSSAO();
        mSSAOOutputTexture.Unbind(mContext.Get());
        mSSAOBlurView.Unbind(mContext.Get(), depthTargetView);


        // Lighting
        mPostProcessView.Use(mContext.Get(), depthTargetView);
        mGBufferTextures.Use(mContext.Get());
        mRenderer->RenderDeferred();
        mGBufferTextures.Unbind(mContext.Get());

        // skybox
        mRenderer->RenderSkybox(reinterpret_cast<IScene*>(mScenes.front().get()));

        // forward
        mContext->OMSetBlendState(mBlendState.Get(), blendFactor, sampleMask);
        mGBufferTextures.Use(mContext.Get());
        mRenderer->RenderForward(scene);
        mGBufferTextures.Unbind(mContext.Get());
        mContext->OMSetBlendState(mBlendStateOff.Get(), blendFactor, sampleMask);
        mPostProcessView.Unbind(mContext.Get(), depthTargetView);

        //Scene* scene2 = reinterpret_cast<Scene*>(scene);
        //std::vector<Camera*>* mCams = scene2->GetMiniCameras();
        //if (mCams->size() > 0)
        //{
        //    D3D11_VIEWPORT vp = (*mCams)[0]->ViewPort();
        //    const D3D11_BOX box = { vp.TopLeftX, vp.TopLeftY, 0, vp.TopLeftX + vp.Width, vp.TopLeftY + vp.Height, 0 };
        //    mContext->CopySubresourceRegion(mDepthStencilBuffer.Get(), 0, vp.TopLeftX, vp.TopLeftY, 0, mMiniDepthStencilBuffer.Get(), 0, &box);
        //}


        //fill mini

        mContext->OMSetDepthStencilState(mDepthStencilState.Get(), 1);

        mPostProcessView.Use(mContext.Get(), depthTargetViewMin);
        mRenderer->RenderMini(scene);
        mPostProcessView.Unbind(mContext.Get(), depthTargetViewMin);
        mContext->OMSetDepthStencilState(mDepthStencilStateSkybox.Get(), 1);


        // bloom
        mBloomBlurView.Use(mContext.Get(), depthTargetView);
        mBloomTexture.Use(mContext.Get());
        mRenderer->BlurTexture(true);
        mBloomTexture.Unbind(mContext.Get());
        mBloomBlurView.Unbind(mContext.Get(), depthTargetView);

        ShaderResourceView blurTex[2] = { mBloomBlurTexture, mBloomBlurStageTexture };
        RenderTargetView blurView[2] = { mBloomBlurStageView, mBloomBlurView };
        bool hor = true;
        for (unsigned i = 0; i < 8; ++i)
        {
            hor = !hor;
            blurView[hor].Use(mContext.Get(), depthTargetView);
            blurTex[hor].Use(mContext.Get());
            mRenderer->BlurTexture(hor);
            blurTex[hor].Unbind(mContext.Get());
            blurView[hor].Unbind(mContext.Get(), depthTargetView);
        }

        // Vignette
        mContext->OMSetBlendState(mBlendState.Get(), blendFactor, sampleMask);
        mVignette.Use(mContext.Get(), depthTargetView);
        mRenderer->RenderToVignette(scene);
        mVignette.Unbind(mContext.Get(), depthTargetView);

        // to screen
        mView.Use(mContext.Get(), depthTargetView);
        mFinalTexture.Use(mContext.Get());
        mRenderer->RenderToScreen();
        mFinalTexture.Unbind(mContext.Get());


    }

    void RenderEngine::EndFrame(float dt)
    {
        mView.Unbind(mContext.Get(), mDepthView.Get()); //unbinding default framebuffer

        Present(false);  //TODO(Seth) no vsync as of yet
    }

    void RenderEngine::Shutdown()
    {
#ifdef _DEBUG
        Microsoft::WRL::ComPtr<ID3D11Debug> debugdev = mDebugDevice.Get();
#endif // DEBUG

        //reset all comptrs for reference counting
        {
            //dev & context
            mDevice.Reset();
            mContext.Reset();

            //resources
            mSwapChain.Reset();
            mDepthView.Reset();
            mMiniDepthView.Reset();

            //states
            mSamplerState[0].Reset();
            mSamplerState[1].Reset();
            mSamplerState[2].Reset();
            mDepthStencilState.Reset();
            mRasterizerState.Reset();
            mBlendState.Reset();

            //loaded assets clear
            mLoadedModels.clear();
            mLoadedShaderPrograms.clear();
        }

        if (mInstance != nullptr)
        {
            delete mInstance;
            mInstance = nullptr;
        }

#ifdef _DEBUG
        if (debugdev != nullptr)
        {
            debugdev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}
#endif // DEBUG

        Utility::COM::__Win32COMCoUninitializeWIC();
        }

    IScene* RenderEngine::CreateScene()
    {
        //TODO(Seth) Garbage
        mScenes.emplace_back(new Scene);
        return mScenes.back().get();
    }

    void RenderEngine::SetActiveScene()
    {
    }

    void RenderEngine::DeleteScene()
    {
    }

    ID3D11ShaderResourceView* RenderEngine::GetTextureView(const wchar_t* f)
    {
        return GetTexture(f, false).Get();
    }

    IShaderProgram* RenderEngine::CreateShaderProgram(char const* vertexShader, char const* pixelShader)
    {
        mLoadedShaderPrograms.emplace_back(ShaderProgram::Create(vertexShader, pixelShader));
        return reinterpret_cast<IShaderProgram*>(mLoadedShaderPrograms.back().get());
    }

    /****************************************************************************/
    /*!
    \brief
        Generate a model class from the providied vertex information

    \param verticies_
        pointer to an array of vertex positions

    \param vertexCount
        the number of verticies

    \param indicies_
        pointer to an array of indicies

    \param indexCount
        the number of indicies

    \param UVs
        pointer to an array of UV coordinates

    \param normals
        optional pointer to an array of normals, will be generated for you if left as null

    \return A pointer to a newly generated IModel class
    */
    /****************************************************************************/
    IModel* RenderEngine::GenerateModel(std::string type, float* verticies_, unsigned vertexCount, unsigned* indicies_,
        unsigned indexCount, float* UVs, float* normals)
    {
        std::vector<Assets::Mesh::SharedPtr> meshes = mLoadedMeshes[type];
        if (meshes.empty())
        {
            std::vector<Vertex> vertices; // holds all vertex info
            vertices.reserve(vertexCount / 3);
            bool useNormals = normals != nullptr; // deduce if normals were provided

            // initialize vertex informaiton
            unsigned j = 0;
            for (unsigned i = 0; i < vertexCount; i += 3, j += 2)
            {
                Vertex vert;

                // precalc array offsets
                unsigned i1 = i + 1;
                unsigned i2 = i + 2;
                unsigned j1 = j + 1;

                /* set vertex  data */
                vert.position = { verticies_[i], verticies_[i1], verticies_[i2] };

                if (useNormals) // use provided normals, else init to zero
                    vert.normal = { normals[i], normals[i1], normals[i2] };
                else
                    vert.normal = { 0,0,0 };

                vert.texCoords = { UVs[j], UVs[j1] };
                vert.tangent = { 0,0,0 };
                vert.bitangent = { 0,0,0 };

                // add vertex to vector
                vertices.push_back(vert);
            }

            // fill the indicie array
            std::vector<unsigned> indicies(indicies_, indicies_ + indexCount);

            // calculate normals if not provided
            if (!useNormals)
                CalculateNormals(&vertices, &indicies);

            // calculate tangents
            CalculateTangents(&vertices, &indicies);

            /* create mesh and model classes */
            meshes.emplace_back(Assets::Mesh::Create(vertices, indicies));
            mLoadedMeshes[type] = meshes;
        }
        mLoadedModels.emplace_back(new Assets::Model(std::forward< std::vector<Assets::Mesh::SharedPtr>>(meshes)));
        return reinterpret_cast<IModel*>(mLoadedModels.back().get());
    }

    ShaderResourceView RenderEngine::LoadCubeMap(const wchar_t* top_, const wchar_t* bottom_, const wchar_t* left_, const wchar_t* right_, const wchar_t* front_, const wchar_t* back_)
    {
        ShaderResourceView srv[6];
        ShaderResourceView cubeMap;

        srv[2] = GetTexture(top_, true);
        srv[3] = GetTexture(bottom_), true;
        srv[1] = GetTexture(left_, true);
        srv[0] = GetTexture(right_, true);
        srv[4] = GetTexture(front_, true);
        srv[5] = GetTexture(back_, true);

        D3D11_SHADER_RESOURCE_VIEW_DESC cubeMapDesc;
        ZeroMemory(&cubeMapDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        D3D11_TEXTURE2D_DESC desc;
        srv[0].GetTexture()->GetDesc(&desc);
        cubeMapDesc.Format = desc.Format;
        cubeMapDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        cubeMapDesc.TextureCube.MipLevels = desc.MipLevels;
        cubeMapDesc.TextureCube.MostDetailedMip = 0;

        D3D11_TEXTURE2D_DESC texArrayDesc;
        ZeroMemory(&texArrayDesc, sizeof(D3D11_TEXTURE2D_DESC));
        texArrayDesc.Width = desc.Width;
        texArrayDesc.Height = desc.Height;
        texArrayDesc.MipLevels = desc.MipLevels;
        texArrayDesc.ArraySize = 6;
        texArrayDesc.Format = desc.Format;
        texArrayDesc.SampleDesc.Count = 1;
        texArrayDesc.SampleDesc.Quality = 0;
        texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
        texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texArrayDesc.CPUAccessFlags = 0;
        texArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

        Texture texArray;
        texArray.Generate(mDevice.Get(), texArrayDesc);
        D3D11_BOX sourceRegion;
        sourceRegion.left = 0;
        sourceRegion.right = 0;
        sourceRegion.top = 0;
        sourceRegion.bottom = 0;
        sourceRegion.front = 0;
        sourceRegion.back = 1;

        for (UINT i = 0; i < 6; ++i)
        {
            for (UINT mipLevel = 0; mipLevel < 12; ++mipLevel)
            {
                sourceRegion.right = (texArrayDesc.Width >> mipLevel);
                sourceRegion.bottom = (texArrayDesc.Height >> mipLevel);

                //test for overflow
                if (sourceRegion.bottom == 0 || sourceRegion.right == 0)
                    break;

                mContext->CopySubresourceRegion(texArray.Get(), D3D11CalcSubresource(mipLevel, i,
                    texArrayDesc.MipLevels), 0, 0, 0, srv[i].GetTexture(), mipLevel, &sourceRegion);
            }
        }

        cubeMap.Generate(mDevice.Get(), texArray, cubeMapDesc);

        return cubeMap;
    }

    ShaderResourceView RenderEngine::LoadTexture(const wchar_t* filename, bool useSRGB)
    {
        ShaderResourceView tex;
        Rendering::RenderEngine* engine = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance());
        HRESULT hr;

        if (useSRGB)
            hr = DirectX::CreateWICTextureFromFileEx(mDevice.Get(), mContext.Get(), filename, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, D3D11_USAGE_DEFAULT,
                D3D11_RESOURCE_MISC_GENERATE_MIPS, DirectX::WIC_LOADER_FORCE_SRGB, reinterpret_cast<ID3D11Resource**>(tex.GetAddressOfTexture()), tex.GetAddressOf());
        else
            hr = DirectX::CreateWICTextureFromFileEx(mDevice.Get(), mContext.Get(), filename, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, D3D11_USAGE_DEFAULT,
                D3D11_RESOURCE_MISC_GENERATE_MIPS, DirectX::WIC_LOADER_IGNORE_SRGB, reinterpret_cast<ID3D11Resource**>(tex.GetAddressOfTexture()), tex.GetAddressOf());

        if (FAILED(hr))
        {
            __debugbreak();
        }

        mLoadedTextures[std::wstring(filename)] = tex;
        return tex;
    }

    ShaderResourceView RenderEngine::GetTexture(const wchar_t* filename, bool useSRGB)
    {
        std::wstring s(filename);

        if (mLoadedTextures.find(s) == mLoadedTextures.end())
        {
            return LoadTexture(filename, useSRGB);
        }

        return mLoadedTextures.at(s);
    }

    ID3D11SamplerState** RenderEngine::GetSampler(unsigned type)
    {
        return mSamplerState[type].GetAddressOf();
    }

    ID3D11Device* RenderEngine::Device()
    {
        return mDevice.Get();
    }

    ID3D11DeviceContext* RenderEngine::Context()
    {
        return mContext.Get();
    }

    ID3D11Device* RenderEngine::Device() const
    {
        return mDevice.Get();
    }

    //private functions
    HRESULT RenderEngine::DisableAltEnter(bool disable) const
    {
        // make screen association to change associated dxgi flags
        Microsoft::WRL::ComPtr<IDXGIDevice> pDXGIDevice;
        HRESULT hr = mDevice.As(&pDXGIDevice);

        Microsoft::WRL::ComPtr<IDXGIAdapter> pDXGIAdapter;
        hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(pDXGIAdapter.GetAddressOf()));

        Microsoft::WRL::ComPtr<IDXGIFactory> pIDXGIFactory;
        hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pIDXGIFactory.GetAddressOf()));

        hr = pIDXGIFactory->MakeWindowAssociation(Utility::GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);

        return hr;
    }

    void RenderEngine::Present(bool vsync) const
    {
        if (vsync)
        {
            //TODO(Seth) Vsync
        }
        else
        {
            mSwapChain->Present(0, 0);
        }
    }

    void RenderEngineResizeCallBack()
    {

    }
    }

static void UpdateAppProjectionMat()
{
    UINT width, height;
    height = Utility::GetWindowHeight();
    width = Utility::GetWindowWidth();

    appProjection = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), (float)width / height, 0.1f, 1000.0f));
}