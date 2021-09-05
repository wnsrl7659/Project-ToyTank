/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

#include "RenderTypes.h"

namespace Rendering {
    class IShaderProgram;
}

namespace Rendering {
    class IModel
    {
    public:
        using InstanceID = size_t;

        enum TEXTURETYPES
        {
            _ALBEDO_ = 0,
            _NORMAL_ = 1,
            _METALIC_ = 3,
            _ROUGH_ = 2,
            _AMBIENTOCCLUSION_ = 4,
            _HEIGHT_ = 5,
            _TEXTURETYPESCOUNT_ = 6
        };

        enum TEXTURESAMPLERSETTINGS
        {
            _WRAP_ = 0,
            _MIRROR_ = 1,
            _CLAMP_ = 2
        };

        virtual ~IModel() = default;
        virtual ModelID ID() const = 0;
        virtual std::string FragmentShader() = 0;
        virtual void SetShaderProgram(IShaderProgram* shaderProgram) = 0;
        virtual void SetTransform(InstanceID id, float const* transform = nullptr) = 0;
        virtual void SetColor(InstanceID id, float r, float g, float b) = 0;
        virtual std::vector<float> GetColor(InstanceID id) = 0;
        virtual void SetAlpha(InstanceID id, float a) = 0;
        virtual void SetTexture(TEXTURETYPES type, const wchar_t* filePath, bool useSRGB = false) = 0;
        virtual void SetTexCoordScale(float scale) = 0;
        virtual void SetSamplerType(TEXTURESAMPLERSETTINGS type) = 0;
        virtual InstanceID AddInstance(const float* transform) = 0;
        virtual void RemoveInstance(InstanceID id) = 0;
        virtual void SetMaterial() = 0;
        virtual void SetSeeThrough(bool canBeSeenThrough) = 0;
        virtual void Renderable(InstanceID id, bool renderable) = 0;

    private:
    };
} // namespace Rendering

//create a model. The first call to the engine to create a model allocates buffers but, nothing
//is going to be rendered as there are no instances.

//add the desired shader program to the model

//create an instance. Use the main IModel* that was returned on model creation to access the API
//calls using any instance IDs as parameters to these calls.
