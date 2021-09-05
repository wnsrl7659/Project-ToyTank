/**
 * @file Window.cpp
 *
 * @brief An implementation of Window.h
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include "window.h"
#include "app.h"
#include "ServiceLocator.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "../../PARender/inc/PARenderAPI/IRenderEngine.h"
#include "../../PARender/inc/PARenderPCH.h"

#include <exception>
#include <stdexcept>
#include <sstream>

namespace
{
	// to forward messages from the inside of Window class to the window procedure
	PAEngine::Window* window = NULL;
}

unsigned GetClientHeight()
{
    RECT desktop;
    const HWND hWindow = GetDesktopWindow();
    GetClientRect(hWindow, &desktop);

    return desktop.bottom;
}

unsigned GetClientWidth()
{
    RECT desktop;
    const HWND hWindow = GetDesktopWindow();
    GetClientRect(hWindow, &desktop);

    return desktop.right;
}

namespace PAEngine
{
	// the window procedure
	LRESULT CALLBACK mainWndProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
	{
		// forward messages from a global window procedure to the member window procedure until a valid window handle is available 
		// this is needed because we can't assign a member function to WINDCLASS:lpfnWndProc
		return window->WndProc(hWnd, msg, wParam, lParam);
	}

	Window::Window(App* _pApp) : mainWindow(NULL), m_pApp(_pApp)
	{
		window = this;  // to forward messages from "WndProc" to "mainWndProc"


#if _DEBUG
        clientWidth = 1280;
        clientHeight = 720;
#else
        clientWidth = GetClientWidth();
        clientHeight = GetClientHeight();
#endif // DEBUG


		try {
			this->Init();
			Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "Window was initialized and created.");
		}
		catch (std::exception& e)
		{
			// create and print error message string
			std::stringstream errorMessage;
			errorMessage << "Creation of the game window failed with: " << e.what();

			Util::ServiceLocator::getLogger()->print(Util::SeverityType::error, "Window Creation was failed.");
			throw std::runtime_error("Window creation failed!");
		}
	}

	Window::~Window()
	{
		if (mainWindow)
			mainWindow = NULL;

		if (m_pApp)
			m_pApp = NULL;
	}

	bool Window::Init()
	{
		WCHAR className[64] = L"Programmer Art Engine"; // to distingush from other programs
		WCHAR windowName[64] = L"PAEngine";     // just a name on the upper-side of this window

		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = mainWndProc; // The window procedure
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = m_pApp->GetHInstance();
		wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcex.lpszMenuName = 0;
		wcex.lpszClassName = className;

		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hIcon = LoadIcon(m_pApp->GetHInstance(), IDI_APPLICATION);
		wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

		if (!RegisterClassEx(&wcex))
		{
			Util::ServiceLocator::getLogger()->print(Util::SeverityType::error, "RegisterClassEx was failed.");
			return FALSE;
		}

		// WindowExA: Ascii
		// WindowExW: Unicode (UTF-16 == wide characters)
		mainWindow = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, wcex.lpszClassName, windowName, (WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU),
			CW_USEDEFAULT, CW_USEDEFAULT, clientWidth, clientHeight,
			nullptr, nullptr, m_pApp->GetHInstance(), nullptr);

		if (!mainWindow)
		{
			Util::ServiceLocator::getLogger()->print(Util::SeverityType::error, "CreateWindowExW was failed.");
			return FALSE;
		}

		ShowWindow(mainWindow, SW_SHOW);
		UpdateWindow(mainWindow);

		return TRUE;
	}

  IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    if (ImGui::GetCurrentContext() == NULL)
      return 0;

    ImGuiIO& io = ImGui::GetIO();
    

    switch (msg)
    {
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    {
      int button = 0;
      if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
      if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
      if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
      if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
      if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
        ::SetCapture(hwnd);
      io.MouseDown[button] = true;
      return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    {
      int button = 0;
      if (msg == WM_LBUTTONUP) { button = 0; }
      if (msg == WM_RBUTTONUP) { button = 1; }
      if (msg == WM_MBUTTONUP) { button = 2; }
      if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
      io.MouseDown[button] = false;
      if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
        ::ReleaseCapture();
      return 0;
    }
    case WM_MOUSEWHEEL:
      io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
      return 0;
    case WM_MOUSEHWHEEL:
      io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
      return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
      if (wParam < 256)
        io.KeysDown[wParam] = 1;
      return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
      if (wParam < 256)
        io.KeysDown[wParam] = 0;
      return 0;
    case WM_CHAR:
      // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
      io.AddInputCharacter((unsigned int)wParam);
      return 0;
      /*
    case WM_SETCURSOR:
      if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
        return 1;
      return 0;
    case WM_DEVICECHANGE:
      if ((UINT)wParam == DBT_DEVNODES_CHANGED)
        g_WantUpdateHasGamepad = true;
      return 0;
      */
    }
    return 0;
  }

  LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
      
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
      return true;
    


    switch (message)
		{
		case WM_DESTROY:
			Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "WM_DESTROY was called in WndProc.");
			PostQuitMessage(0);
			break;
    case WM_ACTIVATE:
      if (LOWORD(wParam) == WA_INACTIVE)	// if the window became inactive, pause the application
      {
        m_pApp->isDisabled = true;
        //m_pApp->timer->stop();
      }
      else                                // if the window was activated, unpause the applcation
      {
        //if (m_pApp->hasStarted)
        //  m_pApp->timer->start();		// needed to prevent memory leak
        m_pApp->isDisabled = false;
      }
      break;
    case WM_SIZE:
      if (wParam == SIZE_MINIMIZED)
      {
        // when the window is minimized, set the appropriate window flags and pause the application
        isMinimized = true;
        isMaximized = false;
        m_pApp->isDisabled = true;
      }
      else if (wParam == SIZE_MAXIMIZED)
      {
        // when the window is maximized, set the appropriate window flags, resize the graphics and unpause the application
        isMinimized = false;
        isMaximized = true;
        //m_pApp->onResize();
        m_pApp->isDisabled = false;
      }
      else if (wParam == SIZE_RESTORED)
      {
        if (isMinimized)
        {
          // the window was restored and was previously minimized, thus we set minimized to false, resize the graphics and restart the application
          isMinimized = false;
          //m_pApp->onResize();
          m_pApp->isDisabled = false;
        }
        else if (isMaximized)
        {
          // the window was resized and was previously maxmized, thus we set maximized to false, resize the graphics and unpause the game
          isMaximized = false;
         // m_pApp->onResize();
          m_pApp->isDisabled = false;
        }
        else if (isResizing)
        {
          // do nothing until the dragging / resizing has stopped (dragging continously sents WM_SIZE messages, it would be extremely slow (and very pointless) to respond to all them)

        }
        //else // resize graphics
        //  m_pApp->onResize();
      }
      break;
    case WM_ENTERSIZEMOVE:
      // the game window is being dragged around, set the isResizing flag and pause the game
      //isResizing = true;
      m_pApp->isDisabled = true;
      //m_pApp->timer->stop();
      break;
    case WM_EXITSIZEMOVE:

        Rendering::RenderEngineResizeCallBack();
      // the game window is no longer being dragged around, set the isResizing flag to false, resize the graphics and unpause the game
      isResizing = false;
      //m_pApp->onResize();
      m_pApp->isDisabled = false;
      //m_pApp->timer->start();
      break;
    case WM_GETMINMAXINFO:
      // prevent the window from becoming too small
      // SVGA	4 : 3
      ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 800;
      ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 600;
      break;
		}

		// As a default, it handles other messages
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}