/******************************************************************************
  * Author : Junki Kim
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once
#include <string>
#include <vector>
#include <d3d11.h>

class PAGuiWindow;

class PAGui {
public:
    PAGui();
    ~PAGui();

    // for testign this will build the window, later that shoudl be moved
    // to a builder/factory class
    // parameters might change when integrated with the engine
    void Init(void* hwnd, ID3D11Device* g_pd3dDevice, ID3D11DeviceContext* g_pd3dDeviceContext);

    // iterates through and updates each window
    void Update();
    // renders the gui, should be called after graphics is done rendering
    void Render();
    void Shutdown();

private:
  std::vector<PAGuiWindow*> guiWindows;
  std::vector<PAGuiWindow*> pressWindows;
  std::vector<PAGuiWindow*> menuWindows;
  std::vector<PAGuiWindow*> winWindows;
  std::vector<PAGuiWindow*> finishWindows;
  std::vector<PAGuiWindow*> uiWindows;
  std::vector<PAGuiWindow*> pauseWindows;
  std::vector<PAGuiWindow*> logoWindows;
  std::vector<PAGuiWindow*> confirmWindows;
  std::vector<PAGuiWindow*> connectWindows;
};

extern bool pressGUI;
extern bool menuGUI;
extern bool winGUI;
extern bool confirmGUI;