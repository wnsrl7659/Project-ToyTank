#include "pch.h"

#include "PAGui.h"
#include "PAGuiWindow.h"
#include "PAGuiWidgets.h"
#include "imgui.h"
#include "EventSystem.h"
#include "App.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "../../PARender/inc/PARenderPCH.h"

static bool drawingGUI = false;
static bool uiGUI = false;
static bool logoGUI = false;
static bool barGUI = false;
static bool finishGUI = false;
static bool connectGUI = false;
bool winGUI = false;
bool menuGUI = false;
bool pressGUI = false;
bool confirmGUI = false;

static void OnOff(void* data)
{
  drawingGUI = !drawingGUI;
}

static void menuOnOff(void* data)
{
  menuGUI = !menuGUI;
}
static void winOnOff(void* data)
{
  winGUI = !winGUI;
}

static void uiOnOff(void* data)
{
	uiGUI = !uiGUI;
}

static void pauseOnOff(void* data)
{
  PAEngine::App::globalApp->isPaused = !PAEngine::App::globalApp->isPaused;
}

static void logoOnOff(void* data)
{
  logoGUI = !logoGUI;
}

static void pressOnOff(void* data)
{
    pressGUI = !pressGUI;
}


static void finishOnOff(void* data)
{
    finishGUI = !finishGUI;
}

static void confirmOnOff(void* data)
{
    confirmGUI = !confirmGUI;
}

//EventSystem::SendEvent(ET_CONNECTONOFF, (void*)nullptr, sizeof(nullptr));
static void connectOnOff(void* data)
{
    connectGUI = !connectGUI;
}


PAGui::PAGui(){
  EventSystem::RegisterEvent(ET_GUIONOFF, (EventHandler)OnOff);
  EventSystem::RegisterEvent(ET_MENUONOFF, (EventHandler)menuOnOff);
  EventSystem::RegisterEvent(ET_WINONOFF, (EventHandler)winOnOff);
  EventSystem::RegisterEvent(ET_UIONOFF, (EventHandler)uiOnOff);
  EventSystem::RegisterEvent(ET_PAUSEONOFF, (EventHandler)pauseOnOff);
  EventSystem::RegisterEvent(ET_LOGOONOFF, (EventHandler)logoOnOff);
  EventSystem::RegisterEvent(ET_PRESSONOFF, (EventHandler)pressOnOff);
  EventSystem::RegisterEvent(ET_FINISHONOFF, (EventHandler)finishOnOff);
  EventSystem::RegisterEvent(ET_CONFIRMONOFF, (EventHandler)confirmOnOff);
  EventSystem::RegisterEvent(ET_CONNECTONOFF, (EventHandler)connectOnOff);

}

PAGui::~PAGui(){
}

void PAGui::Init(void* hwnd, ID3D11Device* g_pd3dDevice, ID3D11DeviceContext* g_pd3dDeviceContext){
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    auto& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(0.860f, 0.930f, 0.890f, 0.78f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.860f, 0.930f, 0.890f, 0.28f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.200f, 0.220f, 0.270f, 0.58f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.200f, 0.220f, 0.270f, 0.9f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.200f, 0.220f, 0.270f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.455f, 0.198f, 0.301f, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.232f, 0.201f, 0.271f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.502f, 0.075f, 0.256f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.200f, 0.220f, 0.270f, 0.75f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.200f, 0.220f, 0.270f, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.200f, 0.220f, 0.270f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.455f, 0.198f, 0.301f, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.455f, 0.198f, 0.301f, 0.86f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.455f, 0.198f, 0.301f, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.455f, 0.198f, 0.301f, 0.86f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.502f, 0.075f, 0.256f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.455f, 0.198f, 0.301f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.860f, 0.930f, 0.890f, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.860f, 0.930f, 0.890f, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.455f, 0.198f, 0.301f, 0.43f);
    style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.200f, 0.220f, 0.270f, 0.73f);

    style.WindowPadding = ImVec2(6, 4);
    style.WindowRounding = 0.0f;
    style.FramePadding = ImVec2(5, 2);
    style.FrameRounding = 3.0f;
    style.ItemSpacing = ImVec2(7, 1);
    style.ItemInnerSpacing = ImVec2(1, 1);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 6.0f;
    style.ScrollbarSize = 12.0f;
    style.ScrollbarRounding = 16.0f;
    style.GrabMinSize = 20.0f;
    style.GrabRounding = 2.0f;

    style.WindowTitleAlign.x = 0.50f;

    style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // test window
    guiWindows.push_back(new PAGuiTestWindow());
    guiWindows.push_back(new PAGuiFPSWindow());
    
    pressWindows.push_back(new PAGuiPressWindow());
    menuWindows.push_back(new PAGuiMenuWindow());

    winWindows.push_back(new PAGuiWinWindow());

	uiWindows.push_back(new PAGuiUIWindow());
	uiWindows.push_back(new PAGuiBarWindow());
    uiWindows.push_back(new PAGuiHPP1Window());
    uiWindows.push_back(new PAGuiHPP2Window());
    uiWindows.push_back(new PAGuiHPP3Window());
    uiWindows.push_back(new PAGuiHPP4Window());
    uiWindows.push_back(new PAGuiFuelP1Window());
    uiWindows.push_back(new PAGuiFuelP2Window());
    uiWindows.push_back(new PAGuiFuelP3Window());
    uiWindows.push_back(new PAGuiFuelP4Window());


    pauseWindows.push_back(new PAGuiPauseWindow());

    logoWindows.push_back(new PAGuiLogoWindow());

    finishWindows.push_back(new PAGuiFinishWindow());

    confirmWindows.push_back(new PAGuiConfirmWindow());

    connectWindows.push_back(new PAGuiConnectWindow());
}

void PAGui::Update() {

  // Start the Dear ImGui frame
  if (drawingGUI || menuGUI || winGUI || uiGUI || logoGUI || pressGUI || finishGUI || confirmGUI || connectGUI || PAEngine::App::globalApp->isPaused)
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGuiIO& io = ImGui::GetIO();

        /*********************************************************/
        int width = Utility::GetWindowWidth();
        int height = Utility::GetWindowHeight();
        RECT desktop;
        // Get a handle to the desktop window
        const HWND hDesktop = GetDesktopWindow();
        // Get the size of screen to the variable desktop
        GetWindowRect(hDesktop, &desktop);
        // The top left corner will have coordinates (0,0)
        // and the bottom right corner will have coordinates
        // (horizontal, vertical)
        int W = desktop.right;
        int H = desktop.bottom;

        io.MousePos.x = io.MousePos.x * (width / io.DisplaySize.x);
        io.MousePos.y = io.MousePos.y * (height / io.DisplaySize.y);

        io.DisplaySize = ImVec2(width, height);
        io.DisplayFramebufferScale = ImVec2(1, 1);

        /*********************************************************/

        ImGui::NewFrame();

        if (drawingGUI)
        {
          for (PAGuiWindow* window : guiWindows) {
            window->Update();
          }
        }
        if (pressGUI)
        {
          for (PAGuiWindow* window : pressWindows) {
            window->Update();
          }
        }
        if (menuGUI)
        {
          for (PAGuiWindow* window : menuWindows) {
            window->Update();
          }
        }
        if (winGUI)
        {
          for (PAGuiWindow* window : winWindows) {
            window->Update();
          }
        }
        if (uiGUI)
        {
          for (PAGuiWindow* window : uiWindows) {
            window->Update();
          }
        }
        if (logoGUI)
        {
          for (PAGuiWindow* window : logoWindows) {
            window->Update();
          }
        }
        if (finishGUI)
        {
            for (PAGuiWindow* window : finishWindows) {
                window->Update();
            }
        }
        if (confirmGUI)
        {
            for (PAGuiWindow* window : confirmWindows) {
                window->Update();
            }
        }
        if (connectGUI)
        {
            for (PAGuiWindow* window : connectWindows) {
                window->Update();
            }
        }
        if (PAEngine::App::globalApp->isPaused)
        {
          for (PAGuiWindow* window : pauseWindows) {
            window->Update();
          }
        }
    }
}

void PAGui::Render(){
    if (drawingGUI || menuGUI || winGUI || uiGUI || logoGUI || pressGUI || finishGUI || confirmGUI || connectGUI || PAEngine::App::globalApp->isPaused)
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
}

void PAGui::Shutdown(){
  for (PAGuiWindow* window : guiWindows) {
    delete window;
  }
  for (PAGuiWindow* window : menuWindows) {
    delete window;
  }
  for (PAGuiWindow* window : winWindows) {
    delete window;
  }
  for (PAGuiWindow* window : uiWindows) {
    delete window;
  }
  for (PAGuiWindow* window : pauseWindows) {
    delete window;
  }
  for (PAGuiWindow* window : logoWindows) {
    delete window;
  }
  for (PAGuiWindow* window : pressWindows) {
    delete window;
  }
  for (PAGuiWindow* window : finishWindows) {
      delete window;
  }
  for (PAGuiWindow* window : confirmWindows) {
      delete window;
  }
  for (PAGuiWindow* window : connectWindows) {
      delete window;
  }
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
