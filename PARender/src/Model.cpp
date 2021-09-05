#include "PARenderPCH.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "RenderEngine.h" //TODO(SETH) TEMPORARARY; FIX ACCESS PATTERNS

namespace Rendering {
    namespace Assets {

        Model::Model(std::vector<Rendering::Assets::Mesh::SharedPtr>&& meshes) : mShaderProgram(nullptr)
        {
            std::swap(mMeshes, meshes);
            mInstances.reserve(MAX_INSTANCE_COUNT / 2);

            //TODO(SETH) SUPER EXTRA HELLA TEMPORARY
            RenderEngine* engine = reinterpret_cast<RenderEngine*>(RenderEngine::GetInstance());
            ID3D11Device* device = engine->Device();
            assert(device != nullptr);

            // Create an initialize the vertex buffer.
            D3D11_BUFFER_DESC instanceBufferDesc{};
            instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            instanceBufferDesc.ByteWidth = static_cast<UINT>(MAX_INSTANCE_COUNT * sizeof(DirectX::XMMATRIX));
            instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;

            HRESULT hr = device->CreateBuffer(&instanceBufferDesc, nullptr, __instanceBuffer.ReleaseAndGetAddressOf());
            if (FAILED(hr))
            {
                __debugbreak();
            }

            hr = device->CreateBuffer(&instanceBufferDesc, nullptr, __instanceNormalBuffer.ReleaseAndGetAddressOf());
            if (FAILED(hr))
            {
                __debugbreak();
            }

            instanceBufferDesc.ByteWidth = static_cast<UINT>(MAX_INSTANCE_COUNT * sizeof(DirectX::XMFLOAT4));
            hr = device->CreateBuffer(&instanceBufferDesc, nullptr, __instanceColorBuffer.ReleaseAndGetAddressOf());
            if (FAILED(hr))
            {
                __debugbreak();
            }

            mTextures.Resize(_TEXTURETYPESCOUNT_);
            mTextures.Set(_ALBEDO_, engine->GetTexture(L"../PAEngine/data/textures/defaultAlbedo.png", true));
            mTextures.Set(_NORMAL_, engine->GetTexture(L"../PAEngine/data/textures/defaultNormal.png"));
            mTextures.Set(_METALIC_, engine->GetTexture(L"../PAEngine/data/textures/defaultBlack.png"));
            mTextures.Set(_ROUGH_, engine->GetTexture(L"../PAEngine/data/textures/defaultBlack.png"));
            mTextures.Set(_AMBIENTOCCLUSION_, engine->GetTexture(L"../PAEngine/data/textures/defaultWhite.png"));
            mTextures.Set(_HEIGHT_, engine->GetTexture(L"../PAEngine/data/textures/defaultBlack.png"));
        }

        ModelID Rendering::Assets::Model::ID() const
        {
            return mID;
        }

        void Model::SetShaderProgram(IShaderProgram* shaderProgram)
        {
            fragPath = shaderProgram->FragmentShader();
            mShaderProgram = reinterpret_cast<ShaderProgram*>(shaderProgram);
        }

        //if transform is nullptr defaults to identity
        Model::InstanceID Rendering::Assets::Model::AddInstance(const float* transform)
        {
            //assert(mInstances.size() < MAX_INSTANCE_COUNT);

            for (auto& instance : mInstances)
            {
                if (instance.__enabled == false)
                {
                    if (transform != nullptr)
                    {
                        __instanceMatricies[instance.__ID] = DirectX::XMMATRIX(transform);
                        __instanceColors[instance.__ID] = DirectX::XMFLOAT4(1, 1, 1, 1);
                    }
                    else
                    {
                        __instanceMatricies[instance.__ID] = DirectX::XMMatrixIdentity();
                        __instanceColors[instance.__ID] = DirectX::XMFLOAT4(1, 1, 1, 1);
                    }

                    instance.__enabled = true; // its wrong. didnt change member value
                    mInstances[instance.__ID].__enabled = true;
                    return instance.__ID;
                }
            }

            Instance instance{};
            instance.__enabled = true;
            instance.__ID = mInstances.size();

            mInstances.push_back(instance);

            if (transform != nullptr)
            {
                __instanceMatricies.push_back(DirectX::XMMATRIX(transform));
                __instanceColors.push_back(DirectX::XMFLOAT4(1, 1, 1, 1));
            }
            else
            {
                __instanceMatricies.push_back(DirectX::XMMatrixIdentity());
                __instanceColors.push_back(DirectX::XMFLOAT4(1, 1, 1, 1));
            }


            return mInstances.back().__ID;
        }

        void Model::RemoveInstance(InstanceID id)
        {
            //assert(id < MAX_INSTANCE_COUNT);
            assert(id < mInstances.size());

            mInstances[id].__enabled = false;
            mInstances[id].__renderable = false;
        }

        void Model::SetMaterial()
        {
        }

        void Model::SetSeeThrough(bool canBeSeenThrough)
        {
            mCBData.mSeeThrough = canBeSeenThrough;
        }

        void Model::SetTransform(InstanceID id, float const* transform)
        {
            //assert(id < MAX_INSTANCE_COUNT);
            assert(id < mInstances.size());

            __instanceMatricies[id] = DirectX::XMMATRIX(transform);
        }

        void Model::SetColor(InstanceID id,  float r, float g, float b)
        {
            //assert(id < MAX_INSTANCE_COUNT);
            assert(id < mInstances.size());

            __instanceColors[id] = DirectX::XMFLOAT4(r,g,b, __instanceColors[id].w);
        }

        std::string Model::FragmentShader()
        {
            return fragPath;
        }

        void Model::SetAlpha(InstanceID id, float a)
        {
            //assert(id < MAX_INSTANCE_COUNT);
            assert(id < mInstances.size());

            __instanceColors[id] = DirectX::XMFLOAT4(__instanceColors[id].x, __instanceColors[id].y, __instanceColors[id].z, a);
        }

        void Model::SetTexture(TEXTURETYPES type, const wchar_t* filePath, bool useSRGB)
        {
          //  if (_HEIGHT_)
          //      mCBData.mUseHeightMap = true;

            mTextures.Set(type,
                reinterpret_cast<RenderEngine*>(RenderEngine::GetInstance())->GetTexture(filePath, useSRGB));
        }

        void Model::SetTexCoordScale(float scale)
        {
            mCBData.cbMiscFloats.x = scale;
        }

        void Model::SetSamplerType(TEXTURESAMPLERSETTINGS type)
        {
            mSamplerType = type;
        }

        void Model::Renderable(InstanceID id, bool renderable)
        {
            //(id < MAX_INSTANCE_COUNT);
            assert(id < mInstances.size());

            mInstances[id].__renderable = renderable;
        }

        float Model::GetTexCoordScale() const
        {
            return mCBData.cbMiscFloats.x;
        }

        Model::ConstantBuffferData Model::GetCBData() const
        {
            return mCBData;
        }

        static std::vector<std::vector<DirectX::XMMATRIX>>packedInstanceMats(10, std::vector<DirectX::XMMATRIX>(MAX_INSTANCE_COUNT));
        static std::vector<std::vector<DirectX::XMMATRIX>>packedInstanceNormalMats(10, std::vector<DirectX::XMMATRIX>(MAX_INSTANCE_COUNT));
        static std::vector<std::vector<DirectX::XMFLOAT4>>packedInstanceColors(10, std::vector<DirectX::XMFLOAT4>(MAX_INSTANCE_COUNT));

        void Model::Draw(ShaderProgram* otherShader)
        {
            RenderEngine* engine = reinterpret_cast<RenderEngine*>(RenderEngine::GetInstance());
            ID3D11Device* device = engine->Device();
            assert(device != nullptr);
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
            device->GetImmediateContext(context.ReleaseAndGetAddressOf());
            assert(context != nullptr);

            //pack and push instance data to GPU and then bind to VertexShader
            unsigned instanceCount = 0;

            for (auto const& instance : mInstances)
            {
                if (instance.__enabled && instance.__renderable)
                {
                    packedInstanceMats[instanceCount / MAX_INSTANCE_COUNT][instanceCount % MAX_INSTANCE_COUNT] = DirectX::XMMatrixTranspose(__instanceMatricies[instance.__ID]);
                    packedInstanceNormalMats[instanceCount / MAX_INSTANCE_COUNT][instanceCount % MAX_INSTANCE_COUNT] = DirectX::XMMatrixInverse(
                        &DirectX::XMMatrixDeterminant(__instanceMatricies[instance.__ID]), __instanceMatricies[instance.__ID]);
                    packedInstanceColors[instanceCount / MAX_INSTANCE_COUNT][instanceCount % MAX_INSTANCE_COUNT] = __instanceColors[instance.__ID];
                    ++instanceCount;
                }
            }

            //vertex array setup for IA stage
            std::vector<ID3D11Buffer*> vertexBuffers(1); //first slot reserved for mesh
            std::vector<UINT> strides;
            strides.push_back(sizeof(Vertex));
            std::vector<UINT> offsets;
            offsets.push_back(0);

            //stage instance data
            //TODO(FUTURE) store instance buffers in vector and iterate that
            vertexBuffers.push_back(__instanceBuffer.Get());
            strides.push_back(sizeof(DirectX::XMMATRIX));
            offsets.push_back(0);

            // normal matrix
            vertexBuffers.push_back(__instanceNormalBuffer.Get());
            strides.push_back(sizeof(DirectX::XMMATRIX));
            offsets.push_back(0);

            vertexBuffers.push_back(__instanceColorBuffer.Get());
            strides.push_back(sizeof(DirectX::XMFLOAT4));
            offsets.push_back(0);

            if (otherShader == nullptr)
                mShaderProgram->Bind(context.Get());
            else
                otherShader->Bind(context.Get());

            if (sendTextures)
            {
                mTextures.Use(context.Get());
            }

            context->PSSetSamplers(0, 1, engine->GetSampler(mSamplerType));

            unsigned num_batches = (instanceCount / MAX_INSTANCE_COUNT) + 1;
            int batch_queue = instanceCount;
            unsigned index = 0;

            while(num_batches > 0)
            {
              context->UpdateSubresource(__instanceBuffer.Get(), 0, 0, &packedInstanceMats[index][0], 0, 0);
              context->UpdateSubresource(__instanceNormalBuffer.Get(), 0, 0, &packedInstanceNormalMats[index][0], 0, 0);
              context->UpdateSubresource(__instanceColorBuffer.Get(), 0, 0, &packedInstanceColors[index][0], 0, 0);

              unsigned renderableCount = 0;
              if (batch_queue > MAX_INSTANCE_COUNT)
              {
                renderableCount = MAX_INSTANCE_COUNT;
                batch_queue -= MAX_INSTANCE_COUNT;
              }
              else
              {
                renderableCount = batch_queue;
                batch_queue = 0;
              }

              --num_batches;
              ++index;

              assert(batch_queue >= 0);

                for (auto const& mesh : mMeshes)
                {
                    //get mesh vertex and index data
                    UINT vertCount, indexCount;
                    vertexBuffers[0] = mesh->GetVertexBuffer(vertCount);
                    ID3D11Buffer* meshIndexBuffer = mesh->GetIndexBuffer(indexCount);

                    assert(vertCount > 0 && indexCount > 0);

                    context->IASetVertexBuffers(0, static_cast<UINT>(vertexBuffers.size()), &vertexBuffers[0], &strides[0], &offsets[0]);
                    context->IASetIndexBuffer(meshIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
                    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                context->DrawIndexedInstanced(indexCount, renderableCount, 0, 0, 0);
              }
            }

            // unbind shader resource
            if (sendTextures)
            {
                mTextures.Unbind(context.Get());
            }
        }

        void Model::SendTextures(bool b)
        {
            sendTextures = b;
        }
    }
}
