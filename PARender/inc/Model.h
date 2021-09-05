/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

#include <memory>
#include <vector>

#include "PARenderAPI/IModel.h"
#include "Mesh.h"
#include "MultiShaderResourceView.h"

const unsigned MAX_INSTANCE_COUNT = 1024; //max number of 64 byte matrix per 4096 * 16 CB

namespace Rendering
{
    class ShaderProgram;
}

namespace Rendering {
    namespace Assets {
        class Model : public IModel
        {
        public:

            struct ConstantBuffferData
            {
                DirectX::XMFLOAT4 cbMiscFloats = { 1, 0.01f, 0, 0 };
               // bool mUseHeightMap = false;
                bool mSeeThrough = true;
            };

            using SharedPtr = std::shared_ptr<Model>;
            Model(std::vector<Rendering::Assets::Mesh::SharedPtr>&& meshes);

            virtual ModelID ID() const override;
            virtual void SetShaderProgram(IShaderProgram* shaderProgram) override;
            virtual void SetTransform(InstanceID id, float const* transform) override;
            virtual void SetColor(InstanceID id, float r, float g, float b) override;
            virtual std::string FragmentShader() override;
            virtual std::vector<float> GetColor(InstanceID id) override { return { __instanceColors[id].x, __instanceColors[id].y , __instanceColors[id].z }; }
            virtual void SetAlpha(InstanceID id, float a) override;
            virtual void SetTexture(TEXTURETYPES type, const wchar_t* filePath, bool useSRGB = false) override;
            virtual void SetTexCoordScale(float scale) override;
            virtual void SetSamplerType(TEXTURESAMPLERSETTINGS type) override;
            virtual InstanceID AddInstance(const float* transform = nullptr) override;
            virtual void RemoveInstance(InstanceID id) override;
            virtual void SetMaterial() override;
            virtual void SetSeeThrough(bool canBeSeenThrough) override;
            virtual void Renderable(InstanceID id, bool renderable) override;

            float GetTexCoordScale() const;
            ConstantBuffferData GetCBData() const;

            void Draw(ShaderProgram* otherShader = nullptr);
            void SendTextures(bool b);

        private:

            ConstantBuffferData mCBData;

            struct Instance {
                InstanceID   __ID;
                bool  __enabled;
                bool __renderable;
            };

            std::vector<Instance> mInstances;
            //TODO(SETH)find max size of buffers
            std::vector<DirectX::XMMATRIX> __instanceMatricies;  //16 * 4096 => 128 bit aligned 1024 max
            std::vector<DirectX::XMFLOAT4> __instanceColors;
            Microsoft::WRL::ComPtr<ID3D11Buffer> __instanceBuffer;
            Microsoft::WRL::ComPtr<ID3D11Buffer> __instanceNormalBuffer;
            Microsoft::WRL::ComPtr<ID3D11Buffer> __instanceColorBuffer;
            //geometry data
            std::vector<Rendering::Assets::Mesh::SharedPtr> mMeshes;

            //class data
            ModelID mID = 0;
            ShaderProgram* mShaderProgram;
            MultiShaderResourceView mTextures;
            bool sendTextures = true;
            unsigned mSamplerType = _WRAP_;
            std::string fragPath = {};
        };
    }
} // namespace Rendering::Assets
