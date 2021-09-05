/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

#include "PARenderAPI/IRenderer.h"
#include <d3d11.h>
#include <wrl/client.h>
#include "Camera.h"
#include "Model.h"

namespace Rendering {
    class Renderer : public IRenderer
    {
    public:
        enum ConstantBuffer
        {
            CB_Application,   //application state, window dimensions, projection Matrix
            CB_Cameras,       //camera view matrices sizeof((n * XMMATRIX)) //128bit align
            CB_Frame,         //frame state, updated each frame. Generally updated once and updated at the beginning of each frame
            CB_Object,        //Object level buffer that stores variables that are different for each object being rendered. World martix for example
            CB_Lights,
            CB_SSAOInfo,
            NumConstantBuffers
        };

        Renderer();
        void RenderToScreen();
        void RenderToVignette(IScene* iscene);
        void RenderDeferred();
        void RenderShadowMap(IScene* iscene, ShaderProgram* shader);
        void RenderSkybox(IScene*);
        void RenderSSAO(IScene* iscene, std::vector<DirectX::XMFLOAT4>& mSSAOKernal);
        void SetSSAOInfo(std::vector<DirectX::XMFLOAT4>& mSSAOKernal);
        void BlurTexture(bool vert);
        void BlurSSAO();
        void RenderOutline(IScene* iscene, ShaderProgram* shader);
        void RenderMini(IScene*);
        virtual void Render(IScene*) override;
        void RenderForward(IScene* scene);
        void SetRendererConstantBufferData(void* data, Renderer::ConstantBuffer dest);
        ShaderResourceView GetSkybox();

    private:

        struct SSAOINFORMATION
        {
            DirectX::XMFLOAT4 samples[16];
            DirectX::XMFLOAT4 radiusBiasPower;
            DirectX::XMFLOAT4 screenWidthHeight;
        };

        struct CAMERAINFORMATION
        {            
            DirectX::XMMATRIX viewMTX;
            DirectX::XMFLOAT4 position;
            DirectX::XMFLOAT4 widthHeight;
        };

        SSAOINFORMATION mSSAOInfo;

        void SetCameraData(Camera& cam, bool useOrtho = false);

        D3D11_VIEWPORT fullScreenViewPort;
        Assets::Model* deferredQuad = nullptr;
        Assets::Model* screenQuad = nullptr;
        Assets::Model* ssaoQuad = nullptr;
        Assets::Model* ssaoBlurRealQuad = nullptr;
        Assets::Model* ssaoBlurQuad = nullptr;
        Assets::Model* ssaoBlurQuadHor = nullptr;
        Assets::Model* vignetteQuad = nullptr;
        ID3D11Buffer* mConstantBuffers[ConstantBuffer::NumConstantBuffers];


        ShaderResourceView mSkyBox; 
        Rendering::IShaderProgram* mColorShader;
        Assets::Model* mSkyBoxCube = nullptr;
        Camera mShadowCam;

        Microsoft::WRL::ComPtr<ID3D11Buffer> mAppCB;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mCameras;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mFrameCB;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mObjectCB;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mSSAOCB;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mLightsCB;
    };
}
