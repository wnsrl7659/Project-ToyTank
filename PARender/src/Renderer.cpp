#include "PARenderPCH.h"
#include "Renderer.h"
#include "RenderEngine.h"
#include "Scene.h"
#include "ShaderProgram.h"

Rendering::Renderer::Renderer()
{
    //TODO(SETH) Classes own their own constant buffers
    //allocate constant for app
    RenderEngine* re = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance());
    ID3D11Device* device = re->Device();

    // Create the constant buffers for the variables defined in the vertex shader.
    D3D11_BUFFER_DESC constantBufferDesc{};

    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.ByteWidth = UINT(sizeof(DirectX::XMMATRIX));
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    HRESULT hr = device->CreateBuffer(&constantBufferDesc, nullptr, mAppCB.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        __debugbreak();
    }

    mConstantBuffers[ConstantBuffer::CB_Application] = mAppCB.Get();

    constantBufferDesc.ByteWidth = UINT(sizeof(CAMERAINFORMATION));
    hr = device->CreateBuffer(&constantBufferDesc, nullptr, mCameras.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        __debugbreak();
    }

    mConstantBuffers[ConstantBuffer::CB_Cameras] = mCameras.Get();

    constantBufferDesc.ByteWidth = UINT(sizeof(DirectX::XMFLOAT4) * 2);
    hr = device->CreateBuffer(&constantBufferDesc, nullptr, mObjectCB.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        __debugbreak();
    }

    mConstantBuffers[ConstantBuffer::CB_Object] = mObjectCB.Get();

    constantBufferDesc.ByteWidth = UINT(sizeof(SSAOINFORMATION));
    hr = device->CreateBuffer(&constantBufferDesc, nullptr, mSSAOCB.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        __debugbreak();
    }
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    constantBufferDesc.ByteWidth = UINT(sizeof(Light::__packed_props) * 64 + sizeof(int) * 4); //all lights + a light count
    hr = device->CreateBuffer(&constantBufferDesc, nullptr, mLightsCB.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        __debugbreak();
    }

    mConstantBuffers[ConstantBuffer::CB_Lights] = mLightsCB.Get();

    //if (deferredQuad == nullptr)
    //{
    //    __debugbreak();
    //}

    mConstantBuffers[ConstantBuffer::CB_SSAOInfo] = mSSAOCB.Get();


    float quadVertices[] = {
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f
    };

    float quadTexCoords[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };

    unsigned quadIndicies[] = {
        0, 1, 3, 3, 2, 0
    };

    deferredQuad = reinterpret_cast<Assets::Model*>(re->GenerateModel("Quad", quadVertices, 12, quadIndicies, 6, quadTexCoords));

#if _DEBUG
    Rendering::IShaderProgram* program = re->CreateShaderProgram("..\\PARender\\bin\\Debug\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Debug\\LightingPixelDefault.cso");
#else
    Rendering::IShaderProgram* program = re->CreateShaderProgram("..\\PARender\\bin\\Release\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Release\\LightingPixelDefault.cso");
#endif

    deferredQuad->SetShaderProgram(program);

    std::vector<float> temp(16);
    deferredQuad->AddInstance(temp.data());
    deferredQuad->Renderable(0, true);
    deferredQuad->SendTextures(false);

    ////////
    screenQuad = reinterpret_cast<Assets::Model*>(re->GenerateModel("Quad", quadVertices, 12, quadIndicies, 6, quadTexCoords));

#if _DEBUG
    program = re->CreateShaderProgram("..\\PARender\\bin\\Debug\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Debug\\ToScreenPixel.cso");
#else
    program = re->CreateShaderProgram("..\\PARender\\bin\\Release\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Release\\ToScreenPixel.cso");
#endif

    screenQuad->SetShaderProgram(program);

    screenQuad->AddInstance(temp.data());
    screenQuad->Renderable(0, true);
    screenQuad->SendTextures(false);

    /////////

    vignetteQuad = reinterpret_cast<Assets::Model*>(re->GenerateModel("Quad", quadVertices, 12, quadIndicies, 6, quadTexCoords));

#if _DEBUG
    program = re->CreateShaderProgram("..\\PARender\\bin\\Debug\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Debug\\VignettePixel.cso");
#else
    program = re->CreateShaderProgram("..\\PARender\\bin\\Release\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Release\\VignettePixel.cso");
#endif

    vignetteQuad->SetShaderProgram(program);
    vignetteQuad->AddInstance(temp.data());
    vignetteQuad->Renderable(0, true);
    vignetteQuad->SendTextures(false);

    ////////
    ssaoQuad = reinterpret_cast<Assets::Model*>(re->GenerateModel("Quad", quadVertices, 12, quadIndicies, 6, quadTexCoords));

#if _DEBUG
    program = re->CreateShaderProgram("..\\PARender\\bin\\Debug\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Debug\\SSAOPixel.cso");
#else
    program = re->CreateShaderProgram("..\\PARender\\bin\\Release\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Release\\SSAOPixel.cso");
#endif

    ssaoQuad->SetShaderProgram(program);

    ssaoQuad->AddInstance(temp.data());
    ssaoQuad->Renderable(0, true);
    ssaoQuad->SendTextures(false);

    ////////
    ssaoBlurQuad = reinterpret_cast<Assets::Model*>(re->GenerateModel("Quad", quadVertices, 12, quadIndicies, 6, quadTexCoords));
    ssaoBlurQuadHor = reinterpret_cast<Assets::Model*>(re->GenerateModel("Quad", quadVertices, 12, quadIndicies, 6, quadTexCoords));
    ssaoBlurRealQuad = reinterpret_cast<Assets::Model*>(re->GenerateModel("Quad", quadVertices, 12, quadIndicies, 6, quadTexCoords));

#if _DEBUG
    program = re->CreateShaderProgram("..\\PARender\\bin\\Debug\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Debug\\SSAOBlurPixel.cso");
#else
    program = re->CreateShaderProgram("..\\PARender\\bin\\Release\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Release\\SSAOBlurPixel.cso");
#endif

    ssaoBlurQuad->SetShaderProgram(program);

#if _DEBUG
    program = re->CreateShaderProgram("..\\PARender\\bin\\Debug\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Debug\\SSAOBlurPixelHor.cso");
#else
    program = re->CreateShaderProgram("..\\PARender\\bin\\Release\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Release\\SSAOBlurPixelHor.cso");
#endif
    ssaoBlurQuadHor->SetShaderProgram(program);

#if _DEBUG
    program = re->CreateShaderProgram("..\\PARender\\bin\\Debug\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Debug\\SSAOBlurRealPixel.cso");
#else
    program = re->CreateShaderProgram("..\\PARender\\bin\\Release\\LightingVertexDefault.cso",
        "..\\PARender\\bin\\Release\\SSAOBlurRealPixel.cso");
#endif
    ssaoBlurRealQuad->SetShaderProgram(program);

    ssaoBlurQuad->AddInstance(temp.data());
    ssaoBlurQuad->Renderable(0, true);
    ssaoBlurQuad->SendTextures(false);

    ssaoBlurQuadHor->AddInstance(temp.data());
    ssaoBlurQuadHor->Renderable(0, true);
    ssaoBlurQuadHor->SendTextures(false);

    ssaoBlurRealQuad->AddInstance(temp.data());
    ssaoBlurRealQuad->Renderable(0, true);
    ssaoBlurRealQuad->SendTextures(false);
    ////////
    mSkyBox = re->LoadCubeMap(L"../PAEngine/data/textures/skybox/top.jpg", L"../PAEngine/data/textures/skybox/bottom.jpg",
        L"../PAEngine/data/textures/skybox/left.jpg", L"../PAEngine/data/textures/skybox/right.jpg",
        L"../PAEngine/data/textures/skybox/front.jpg", L"../PAEngine/data/textures/skybox/back.jpg");

#if _DEBUG
    mColorShader = re->CreateShaderProgram("..\\PARender\\bin\\Debug\\GBufferVertex.cso",
        "..\\PARender\\bin\\Debug\\MiniMapPixel.cso");
#else
    mColorShader = re->CreateShaderProgram("..\\PARender\\bin\\Release\\GBufferVertex.cso",
        "..\\PARender\\bin\\Release\\MiniMapPixel.cso");
#endif

    float g_Vertices[] =
    {
        -1.0,  1.0, -1.0,
         1.0,  1.0, -1.0,
        -1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,
         1.0,  1.0, -1.0,
         1.0, -1.0, -1.0,

         1.0,  1.0, -1.0,
         1.0,  1.0,  1.0,
         1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0,  1.0,  1.0,
         1.0, -1.0,  1.0,

         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
         1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
        -1.0,  1.0,  1.0,
        -1.0, -1.0,  1.0,

        -1.0,  1.0,  1.0,
        -1.0,  1.0, -1.0,
        -1.0, -1.0,  1.0,
        -1.0, -1.0,  1.0,
        -1.0,  1.0, -1.0,
        -1.0, -1.0, -1.0,

        -1.0,  1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0,
         1.0,  1.0,  1.0,
         1.0,  1.0, -1.0,

        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
        -1.0, -1.0,  1.0,
        -1.0, -1.0,  1.0,
         1.0, -1.0, -1.0,
         1.0, -1.0,  1.0,
    };

    unsigned g_Indicies[36] =
    {
       35,34,33,32,31,30,
       29,28,27,26,25,24,
       23,22,21,20,19,18,
       17,16,15,14,13,12,
       11,10,9,8,7,6,5,4,
       3,2,1,0
    };

    float g_UV[] =
    {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,
    };

    mSkyBoxCube = reinterpret_cast<Assets::Model*>(re->GenerateModel("Cube", g_Vertices, 108, g_Indicies, 36, g_UV));

#if _DEBUG
    program = re->CreateShaderProgram("..\\PARender\\bin\\Debug\\SkyBoxVertex.cso",
        "..\\PARender\\bin\\Debug\\SkyBoxPixel.cso");
#else
    program = re->CreateShaderProgram("..\\PARender\\bin\\Release\\SkyBoxVertex.cso",
        "..\\PARender\\bin\\Release\\SkyBoxPixel.cso");
#endif

    mSkyBoxCube->SetShaderProgram(program);

    mSkyBoxCube->AddInstance(temp.data());
    mSkyBoxCube->Renderable(0, true);
    mSkyBoxCube->SendTextures(false);

    mShadowCam.SetAspectRatio(500, 500);
    mShadowCam.SetFOV(1.5708f);
    mShadowCam.SetFarPlane(500.f);
    mShadowCam.SetViewPort(4096, 4096, 0, 0, 0, 1);
    mShadowCam.SetPosition(-200, 100, -200);
    mShadowCam.SetFront({ 200, -100, 200 });
    mShadowCam.UseOrthoMatrix(true);
}

void Rendering::Renderer::RenderToScreen()
{
    IEngine* re = Rendering::RenderEngine::GetInstance();
    ID3D11DeviceContext* context = re->Context();
    context->RSSetViewports(1, &fullScreenViewPort);
    screenQuad->Draw();
}

void Rendering::Renderer::RenderToVignette(IScene* iscene)
{
    IEngine* re = Rendering::RenderEngine::GetInstance();
    ID3D11DeviceContext* context = re->Context();
    Scene* scene = reinterpret_cast<Scene*>(iscene);
    std::vector<Camera*>* mCams = scene->GetActiveCameras();
    unsigned viewPortCount = unsigned(mCams->size());
    for (unsigned i = 0; i < viewPortCount; ++i)
    {
        context->RSSetViewports(1, &(*mCams)[i]->ViewPort());
        vignetteQuad->Draw();
    }
}

void Rendering::Renderer::RenderDeferred()
{
    IEngine* re = Rendering::RenderEngine::GetInstance();

    fullScreenViewPort.Width = FLOAT(Utility::GetWindowWidth());
    fullScreenViewPort.Height = FLOAT(Utility::GetWindowHeight());
    fullScreenViewPort.TopLeftX = 0;
    fullScreenViewPort.TopLeftY = 0;
    fullScreenViewPort.MinDepth = 0;
    fullScreenViewPort.MaxDepth = 1;
    ID3D11DeviceContext* context = re->Context();

    //set lighting cb reg(b0)
    SetCameraData(mShadowCam, true);
    context->RSSetViewports(1, &fullScreenViewPort);

    context->PSSetConstantBuffers(2, 1, &mConstantBuffers[ConstantBuffer::CB_Lights]);

    deferredQuad->Draw();
}

void Rendering::Renderer::RenderShadowMap(IScene* iscene, ShaderProgram* shader)
{
    ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(context.ReleaseAndGetAddressOf());
    assert(context != nullptr);

    context->VSSetConstantBuffers(0, NumConstantBuffers, mConstantBuffers);
    context->PSSetConstantBuffers(0, NumConstantBuffers, mConstantBuffers);

    Scene* scene = reinterpret_cast<Scene*>(iscene);

    std::vector<Assets::Model*>sceneModels = scene->GetModels();

    // std::vector<Camera*>* mCams = scene->GetActiveCameras();
    // unsigned viewPortCount = unsigned(mCams->size());
   //  for (unsigned i = 0; i < viewPortCount; ++i)
   //  {


   // std::vector<Camera*>* mCams = scene->GetActiveCameras();
    //(*mCams)[0]->UpdateDirtyMatricies();
    //SetCameraData(*(*mCams)[0]);
   // mShadowCam = *(*mCams)[0];

    //  mShadowCam.SetPitch(-0.261799);
    //  mShadowCam.SetYaw(-0.785398);

    mShadowCam.UpdateDirtyMatricies();
    SetCameraData(mShadowCam, true);

    for (auto const& model : sceneModels)
    {
        Assets::Model::ConstantBuffferData data = model->GetCBData();
        context->UpdateSubresource(mConstantBuffers[CB_Object], 0, nullptr, &data, 0, 0);
        model->Draw(shader);
    }
    //  }
}

void Rendering::Renderer::RenderSkybox(IScene* iscene)
{
    ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(context.ReleaseAndGetAddressOf());

    Scene* scene = reinterpret_cast<Scene*>(iscene);

    std::vector<Assets::Model*>sceneModels = scene->GetModels();

    //update lighting cb for deferred call
    std::vector<Light::__packed_props> sceneLights = scene->GetPackedLights();

    if (sceneLights.size() > 0)
    {
        D3D11_MAPPED_SUBRESOURCE mappedLights{};
        HRESULT hr = context->Map(mLightsCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedLights);
        if (FAILED(hr))
            __debugbreak();

        int count = static_cast<int>(sceneLights.size());
        std::memcpy(mappedLights.pData, &sceneLights[0], sceneLights.size() * sizeof(Light::__packed_props));
        std::memcpy(reinterpret_cast<char*>(mappedLights.pData) + sizeof(Light::__packed_props) * 64, &count, sizeof(int));
        context->Unmap(mLightsCB.Get(), 0);
    }

    std::vector<Camera*>* mCams = scene->GetActiveCameras();
    unsigned viewPortCount = unsigned(mCams->size());
    mSkyBox.Use(context.Get());
    for (unsigned i = 0; i < viewPortCount; ++i)
    {
        (*mCams)[i]->UpdateDirtyMatricies();
        SetCameraData(*(*mCams)[i]);
        mSkyBoxCube->Draw();
    }
    mSkyBox.Unbind(context.Get());
}

void Rendering::Renderer::RenderSSAO(IScene* iscene, std::vector<DirectX::XMFLOAT4>& mSSAOKernal)
{
    IEngine* re = Rendering::RenderEngine::GetInstance();
    ID3D11DeviceContext* context = re->Context();
    context->RSSetViewports(1, &fullScreenViewPort);

    for (unsigned i = 0; i < 16; ++i)
        mSSAOInfo.samples[i] = mSSAOKernal[i];
    mSSAOInfo.radiusBiasPower = { 2.5f, 1.5f, -0.15f, 1 };
    mSSAOInfo.screenWidthHeight = { FLOAT(Utility::GetWindowWidth()), FLOAT(Utility::GetWindowHeight()), 0, 1 };

    context->UpdateSubresource(mConstantBuffers[CB_SSAOInfo], 0, nullptr, &mSSAOInfo, 0, 0);

    //   Scene* scene = reinterpret_cast<Scene*>(iscene);

     //  std::vector<Assets::Model*>sceneModels = scene->GetModels();

     //  std::vector<Camera*>* mCams = scene->GetActiveCameras();
      // unsigned viewPortCount = unsigned(mCams->size());
      // for (unsigned i = 0; i < viewPortCount; ++i)
     //  {
      //     (*mCams)[i]->UpdateDirtyMatricies();
     //      SetCameraData(*(*mCams)[i]);
    ssaoQuad->Draw();
    // }
}

void Rendering::Renderer::SetSSAOInfo(std::vector<DirectX::XMFLOAT4>& mSSAOKernal)
{
    IEngine* re = Rendering::RenderEngine::GetInstance();
    ID3D11DeviceContext* context = re->Context();

    for (unsigned i = 0; i < 16; ++i)
        mSSAOInfo.samples[i] = mSSAOKernal[i];
    mSSAOInfo.radiusBiasPower = { 1.5f, 0.025f, 1.1f, 1 };
    mSSAOInfo.screenWidthHeight = { FLOAT(Utility::GetWindowWidth()), FLOAT(Utility::GetWindowHeight()), 0, 1 };

    context->UpdateSubresource(mConstantBuffers[CB_SSAOInfo], 0, nullptr, &mSSAOInfo, 0, 0);
}

void Rendering::Renderer::BlurTexture(bool vert)
{
    IEngine* re = Rendering::RenderEngine::GetInstance();
    ID3D11DeviceContext* context = re->Context();
    context->RSSetViewports(1, &fullScreenViewPort);

    if(vert)
        ssaoBlurQuad->Draw();
    else
        ssaoBlurQuadHor->Draw();
}

void Rendering::Renderer::BlurSSAO()
{
    IEngine* re = Rendering::RenderEngine::GetInstance();
    ID3D11DeviceContext* context = re->Context();
    context->RSSetViewports(1, &fullScreenViewPort);

    ssaoBlurRealQuad->Draw();
}

void Rendering::Renderer::RenderOutline(IScene* iscene, ShaderProgram* shader)
{
    ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(context.ReleaseAndGetAddressOf());
    assert(context != nullptr);

    context->VSSetConstantBuffers(0, NumConstantBuffers, mConstantBuffers);
    context->PSSetConstantBuffers(0, NumConstantBuffers, mConstantBuffers);

    Scene* scene = reinterpret_cast<Scene*>(iscene);

    std::vector<Assets::Model*>sceneModels = scene->GetModels();

    std::vector<Camera*>* mCams = scene->GetActiveCameras();
    unsigned viewPortCount = unsigned(mCams->size());
    for (unsigned i = 0; i < viewPortCount; ++i)
    {
        (*mCams)[i]->UpdateDirtyMatricies();
        SetCameraData(*(*mCams)[i]);

        for (auto const& model : sceneModels)
        {
            Assets::Model::ConstantBuffferData data = model->GetCBData();
            context->UpdateSubresource(mConstantBuffers[CB_Object], 0, nullptr, &data, 0, 0);
            model->Draw(shader);
        }
    }
}

void Rendering::Renderer::RenderMini(IScene* iscene)
{
    ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(context.ReleaseAndGetAddressOf());
    assert(context != nullptr);

    context->VSSetConstantBuffers(0, NumConstantBuffers, mConstantBuffers);
    context->PSSetConstantBuffers(0, NumConstantBuffers, mConstantBuffers);

    Scene* scene = reinterpret_cast<Scene*>(iscene);

    std::vector<Assets::Model*>sceneModels = scene->GetModels();

    std::vector<Camera*>* mCams = scene->GetMiniCameras();
    unsigned viewPortCount = unsigned(mCams->size());
    for (unsigned i = 0; i < viewPortCount; ++i)
    {
        (*mCams)[i]->UpdateDirtyMatricies();
        SetCameraData(*(*mCams)[i]);

        for (auto const& model : sceneModels)
        {
            Assets::Model::ConstantBuffferData data = model->GetCBData();
            context->UpdateSubresource(mConstantBuffers[CB_Object], 0, nullptr, &data, 0, 0);
            model->Draw(reinterpret_cast<ShaderProgram*>(mColorShader));
        }
    }
}

void Rendering::Renderer::Render(IScene* iscene)
{
    ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(context.ReleaseAndGetAddressOf());
    assert(context != nullptr);

    context->VSSetConstantBuffers(0, NumConstantBuffers, mConstantBuffers);
    context->PSSetConstantBuffers(0, NumConstantBuffers, mConstantBuffers);

    Scene* scene = reinterpret_cast<Scene*>(iscene);

    std::vector<Assets::Model*>sceneModels = scene->GetModels();

    std::vector<Camera*>* mCams = scene->GetActiveCameras();
    unsigned viewPortCount = unsigned(mCams->size());
    for (unsigned i = 0; i < viewPortCount; ++i)
    {
        (*mCams)[i]->UpdateDirtyMatricies();
        SetCameraData(*(*mCams)[i]);

        for (auto const& model : sceneModels)
        {
            Assets::Model::ConstantBuffferData data = model->GetCBData();
            context->UpdateSubresource(mConstantBuffers[CB_Object], 0, nullptr, &data, 0, 0);
            model->Draw();
        }
    }
}

void Rendering::Renderer::RenderForward(IScene* iscene)
{
    ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(context.ReleaseAndGetAddressOf());
    assert(context != nullptr);

    context->VSSetConstantBuffers(0, NumConstantBuffers, mConstantBuffers);
    context->PSSetConstantBuffers(0, NumConstantBuffers, mConstantBuffers);

    Scene* scene = reinterpret_cast<Scene*>(iscene);

    std::vector<Assets::Model*>sceneModels = scene->GetForwardModels();

    if (sceneModels.size() <= 0)
        return;

    std::vector<Camera*>* mCams = scene->GetActiveCameras();
    unsigned viewPortCount = unsigned(mCams->size());
    for (unsigned i = 0; i < viewPortCount; ++i)
    {
        (*mCams)[i]->UpdateDirtyMatricies();
        SetCameraData(*(*mCams)[i]);

        for (auto const& model : sceneModels)
        {
            Assets::Model::ConstantBuffferData data = model->GetCBData();
            context->UpdateSubresource(mConstantBuffers[CB_Object], 0, nullptr, &data, 0, 0);
            model->Draw();
        }
    }
}

void Rendering::Renderer::SetRendererConstantBufferData(void* data, Renderer::ConstantBuffer dest)
{
    ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(context.ReleaseAndGetAddressOf());
    assert(context != nullptr);

    context->UpdateSubresource(mConstantBuffers[dest], 0, nullptr, data, 0, 0);
}

ShaderResourceView Rendering::Renderer::GetSkybox()
{
    return mSkyBox;
}

void Rendering::Renderer::SetCameraData(Camera& cam, bool useOrtho)
{
    ID3D11Device* device = reinterpret_cast<Rendering::RenderEngine*>(Rendering::RenderEngine::GetInstance())->Device();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(context.ReleaseAndGetAddressOf());
    assert(context != nullptr);

    cam.UseViewport(context.Get());
    std::vector<float> p = cam.Position();

    CAMERAINFORMATION cInfo;
    cInfo.viewMTX = DirectX::XMMatrixTranspose(cam.ViewMatrix());
    cInfo.position = { p[0], p[1], p[2], 1 };
    D3D11_VIEWPORT vp = cam.ViewPort();
    cInfo.widthHeight = { FLOAT(vp.Width), FLOAT(vp.Height), FLOAT(-vp.TopLeftX), FLOAT(-vp.TopLeftY) };
    context->UpdateSubresource(mConstantBuffers[CB_Cameras], 0, nullptr, &cInfo, 0, 0);/*
    context->UpdateSubresource(mConstantBuffers[CB_Cameras], 1, nullptr, &DirectX::XMMatrixTranspose(cam.ViewMatrix()), 0, 0);*/

    if (cam.UsingOrtho())
        context->UpdateSubresource(mConstantBuffers[CB_Application], 0, nullptr, &DirectX::XMMatrixTranspose(cam.OrthoMatrix()), 0, 0);
    else
        context->UpdateSubresource(mConstantBuffers[CB_Application], 0, nullptr, &DirectX::XMMatrixTranspose(cam.ProjectionMatrix()), 0, 0);
}