/**
 * @file Window.h
 *
 * @brief It initializes window attribuets, then creates a window for the app
 *  Most of Win32-related functions will be on this class
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

#include <windows.h>

namespace PAEngine
{
	// To avoid duplicated including app.h
	class App;

	class Window
	{
	public:
		Window(App* _pApp);
		~Window();

		inline HWND GetMainWindowHandle() { return mainWindow; }

		// the call back function
		virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		HWND mainWindow;

	private:
		App* m_pApp;

    // resolution
    int clientWidth;
    int clientHeight;

    // window states
    bool isMinimized;
    bool isMaximized;
    bool isResizing;	// when the window is being dragged around by the mouse

		bool Init();
	};
}