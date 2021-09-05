/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

//system include
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

//Graphics Library Includes
//DIRECTX11
#include <d3d11.h>
#include <d3dcompiler.h>    //shader compilation, shader reflection
#include <d3d11shader.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <Objbase.h>        //COM coinitialization

//link library dependencies
//DirectX 11 dependencies
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dCompiler.lib") //shader compilation, shader reflection
#pragma comment(lib, "dxguid.lib")      //IID_ID3D11ShaderReflection GUID
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Ole32.lib")       //COM coinitialization

//STL Includes
#include <algorithm>
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <codecvt>  //string conversion utility functions

//CMath
#define _USE_MATH_DEFINES
#include <math.h>
constexpr float PI = 3.14159265359f;
#undef _USE_MATH_DEFINES

//utilities
//TODO(Seth) Create Utilities file

namespace Utility {
  inline unsigned GetClientWidth()
  {
  RECT desktop;
  const HWND hWnd = GetDesktopWindow();
  GetClientRect(hWnd, &desktop);

  return desktop.right - desktop.left;
  }

  inline unsigned GetClientHeight()
  {
  RECT desktop;
  const HWND hWnd = GetDesktopWindow();
  GetClientRect(hWnd, &desktop);

  return desktop.bottom - desktop.top;
  }

  //Gets the filename from the path and returns the hash of the filename
  inline size_t HashFileName(std::string const& path)
  {
  return std::hash<std::string>{}(path.substr(path.find_last_of("/\\") + 1));
  }

  //just hash the string
  inline size_t HashString(std::string const& string)
  {
  return std::hash<std::string>{}(string);
  }

  //https://codereview.stackexchange.com/a/146738
  inline std::wstring utf8ToUtf16(const std::string& utf8Str)
  {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
  return conv.from_bytes(utf8Str);
  }

  inline std::string utf16ToUtf8(const std::wstring& utf16Str)
  {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
  return conv.to_bytes(utf16Str);
  }

  inline HMODULE GetAppModuleHandle()
  {
  return GetModuleHandle(nullptr);
  }

  inline HWND GetWindowHandle()
  {
  return GetActiveWindow();
  }

  inline UINT GetWindowWidth()
  {
  RECT windowRect{};
  GetWindowRect(GetWindowHandle(), &windowRect);

  return windowRect.right - windowRect.left;
  }

  inline UINT GetWindowHeight()
  {
  RECT windowRect{};
  GetWindowRect(GetWindowHandle(), &windowRect);

  return windowRect.bottom - windowRect.top;
  }

  namespace COM {
    inline void __Win32COMCoInitializeWIC()
    {
      HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
      if (FAILED(hr))
      {
        __debugbreak();
      }
    }

    inline void __Win32COMCoUninitializeWIC()
    {
      CoUninitialize();
    }

  }
}  // namespace Utility
