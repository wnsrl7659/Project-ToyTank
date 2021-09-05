/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

#include <d3d11.h>
#include <string>

namespace Rendering {
    class IRenderer;
    class IScene;
    class IModel;
    class IShaderProgram;
} // namespace Rendering

namespace Rendering {
    void RenderEngineResizeCallBack();

    class IEngine {
    public:
        using WindowCallback = LRESULT(*)(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam);
        struct PARenderInfo
        {
            unsigned  windowWidth;
            unsigned  windowHeight;
            WindowCallback WinProc;
            WCHAR* windowName;
            bool fullscreen;
            bool vsync;
        };

        virtual ~IEngine() = default;
        virtual void Initialize(PARenderInfo renderInfo) = 0;
        virtual void StartFrame(float dt) = 0;
        virtual void RenderScene(float dt) = 0;
        virtual void EndFrame(float dt) = 0;
        virtual void Shutdown() = 0;

        virtual IScene* CreateScene() = 0;
        virtual void SetActiveScene() = 0;
        virtual void DeleteScene() = 0;

        virtual IShaderProgram* CreateShaderProgram(char const* vertexShader, char const* pixelShader) = 0;

        virtual IModel* GenerateModel(std::string type, float* verticies, unsigned vertexCount, unsigned* indicies,
            unsigned indexCount, float* UVs, float* normals = nullptr) = 0;

        virtual ID3D11Device* Device() = 0;
        virtual ID3D11DeviceContext* Context() = 0;

        virtual ID3D11ShaderResourceView* GetTextureView(const wchar_t*) = 0;

    private:

        friend void RenderEngineResizeCallBack();
    };

    extern IEngine* RenderEngineInstance();
} // namespace Rendering
