/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

#include <wrl/client.h>
#include <memory>
#include <vector>

#include "PARenderAPI/IRenderEngine.h"
//#include "Renderer.h" //class Renderer for unique_ptr
#include "MultiRenderTargetView.h"
#include "MultiShaderResourceView.h"
#include "DepthStencilView.h"
#include <unordered_map>
#include <string>
#include "Model.h"

namespace Rendering {
    class Renderer;
    class Scene;
    class Window;
    class ShaderProgram;

    namespace Assets {
        class Model;
    }
}

namespace Rendering {
 
    class RenderEngine : public IEngine
    {
    public:
        virtual void Initialize(PARenderInfo renderinfo) override;
        virtual void StartFrame(float dt) override;
        virtual void RenderScene(float dt) override;
        virtual void EndFrame(float dt) override;
        virtual void Shutdown() override;

        virtual IScene* CreateScene() override;
        virtual void SetActiveScene() override;
        virtual void DeleteScene() override;


        virtual ID3D11ShaderResourceView* GetTextureView(const wchar_t*) override;
        //RenderEngine::GetInstance())->GetTexture(filePath, useSRGB)

        virtual IShaderProgram* CreateShaderProgram(char const* vertexShader, char const* pixelShader) override;

        virtual IModel* GenerateModel(std::string type, float* verticies, unsigned vertexCount, unsigned* indicies,
            unsigned indexCount, float* UVs, float* normals = nullptr) override;
        ShaderResourceView LoadCubeMap(const wchar_t* top, const wchar_t* bottom, const wchar_t* left,
            const wchar_t* right, const wchar_t* front, const wchar_t* back);
        ShaderResourceView LoadTexture(const wchar_t* filename, bool useSRGB = false);
        ShaderResourceView GetTexture(const wchar_t* filename, bool useSRGB = false);
        ID3D11SamplerState** GetSampler(unsigned type);

        virtual ID3D11Device* Device() override;
        virtual ID3D11DeviceContext* Context() override;

        ID3D11Device* Device() const;

        static IEngine* GetInstance();
    private:
     
        friend void RenderEngineResizeCallBack();

        //window class
        Window* mWindow;

        HRESULT DisableAltEnter(bool disable) const;
        void Present(bool vsync) const;

        //device and context
        Microsoft::WRL::ComPtr<ID3D11Device> mDevice = nullptr;
        Microsoft::WRL::ComPtr<ID3D11Debug> mDebugDevice = nullptr;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext = nullptr;

        //resources
        Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain = nullptr;
        DepthStencilView mDepthView;
        DepthStencilView mMiniDepthView;
        DepthStencilView mShadowView;
        RenderTargetView mView;
        MultiRenderTargetView mPostProcessView;
        RenderTargetView mSSAOView;
        RenderTargetView mSSAOBlurView;
        RenderTargetView mBloomBlurView;
        RenderTargetView mBloomBlurStageView;
        RenderTargetView mVignette;
        MultiRenderTargetView mGBuffer;
        MultiShaderResourceView mGBufferTextures;
        MultiShaderResourceView mSSAOTextures;
        Texture mDepthStencilBuffer;
        Texture mMiniDepthStencilBuffer;
        Texture mSwapChainBuffer;
        Texture mPostProcessBuffer;
        Texture mBloomBuffer;
        Texture mBloomBlurBuffer;
        Texture mBloomBlurStageBuffer;
        Texture mNoiseTexture;
        Texture mShadowBuffer;
        Texture mVignetteBuffer;
        MultiShaderResourceView mPostProcessTexture;
        MultiShaderResourceView mFinalTexture;
        ShaderResourceView mVignetteTexture;
        ShaderResourceView mBloomBlurTexture;
        ShaderResourceView mBloomBlurStageTexture;
        ShaderResourceView mBloomTexture;
        ShaderResourceView mSSAONoiseTexture;
        ShaderResourceView mSSAOOutputTexture;
        ShaderResourceView mSSAOOutputBlurTexture;
        ShaderResourceView mShadowTexture;
        ShaderResourceView mOutlineTexture;
        std::unordered_map<std::wstring, ShaderResourceView> mLoadedTextures;
        std::unordered_map<std::string, std::vector<Assets::Mesh::SharedPtr>> mLoadedMeshes;
        std::vector<DirectX::XMFLOAT4>mSSAOKernal;
        ShaderProgram* mShadowShader = nullptr;
        ShaderProgram* mOutlineShader = nullptr;

        //states
        std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> mSamplerState = { nullptr, nullptr, nullptr };
        Microsoft::WRL::ComPtr<ID3D11SamplerState> mShadowSamplerState = nullptr;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState = nullptr;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilStateSkybox = nullptr;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerState = nullptr;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> mShadowRasterizerState = nullptr;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> mOutlineRasterizerState = nullptr;
        Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendState;
        Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendStateOff;

        //renderer
        std::unique_ptr<Renderer> mRenderer;

        //User created scenes are stored here
        std::vector<std::unique_ptr<Scene>> mScenes;

        //TODO(Seth) Internal:: components here
        D3D11_VIEWPORT mViewport{};

        //singleton instance
        static IEngine* mInstance;

        //loaded assets
        std::vector<std::unique_ptr<Assets::Model >> mLoadedModels;
        std::vector<std::unique_ptr<ShaderProgram>>  mLoadedShaderPrograms;
    };

    //return the instance through static class function
    inline IEngine* RenderEngineInstance() {
        return RenderEngine::GetInstance();
    }
}
