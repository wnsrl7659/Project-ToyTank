/**
 * @file PAEngine.cpp
 *
 * @brief This is "Programmer Art Engine" which is called PAEngine for short
 *  It contains an application class which will be running in WinMain 
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>

#include "App.h"

void OpenConsole()
{
  AllocConsole();

  HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  int openHandle = _open_osfhandle((long)outHandle, _O_TEXT);
  FILE* outStream = _fdopen(openHandle, "w");
  freopen_s(&outStream, "CONOUT$", "w", stdout);

  HANDLE inHandle = GetStdHandle(STD_INPUT_HANDLE);
  openHandle = _open_osfhandle((long)inHandle, _O_TEXT);
  FILE* inStream = _fdopen(openHandle, "r");
  freopen_s(&inStream, "CONIN$", "r", stdin);

}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
#if(_DEBUG)
  OpenConsole();
#endif

  PAEngine::App app(hInstance);

  // If initializing app successes, run app
	if (app.Init())
		app.Run();
	else // Something went wrong when initializing app
	{
		app.Shutdown();

    return -1;
  }

  app.Shutdown();

	return 1;
}