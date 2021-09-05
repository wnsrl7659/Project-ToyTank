/******************************************************************************
  * Author : Junki Kim
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once
#include <string>
#include <vector>

#include "ObjectManager.h"
#include "PAGuiWidgets.h"

class PAGuiWidgets;

class PAGuiWindow {
public:
    PAGuiWindow(std::string windowTitle);
    ~PAGuiWindow();

    virtual void Update();

    // the window will delete the memory when it is destroyed
    void AddWidget(PAGuiWidgets* widget);

private:
    std::string title_;
    std::vector<PAGuiWidgets*> widgets_;
};

class PAGuiTestWindow : public PAGuiWindow {
public:
    PAGuiTestWindow();
    ~PAGuiTestWindow();

private:
    bool testCheckBoxBool_ = false;
    int  sliderint_ = 0;
    int  dragint_ = 0;
    int a = 0;
    int b = 0;
    int c = 0;
    bool yes = true;
};

class PAGuiFPSWindow : public PAGuiWindow {
public:
    PAGuiFPSWindow();
    ~PAGuiFPSWindow();

private:
};

class PAGuiMenuWindow : public PAGuiWindow {
public:
  PAGuiMenuWindow();
  ~PAGuiMenuWindow();

private:
};
class PAGuiPressWindow : public PAGuiWindow {
public:
  PAGuiPressWindow();
  ~PAGuiPressWindow();

private:
};

class PAGuiWinWindow : public PAGuiWindow {
public:
  PAGuiWinWindow();
  ~PAGuiWinWindow();

private:
};

class PAGuiUIWindow : public PAGuiWindow {
public:
  PAGuiUIWindow();
  ~PAGuiUIWindow();

private:
};

class PAGuiHPP1Window : public PAGuiWindow {
public:
    PAGuiHPP1Window();
    ~PAGuiHPP1Window();

private:
};

class PAGuiHPP2Window : public PAGuiWindow {
public:
    PAGuiHPP2Window();
    ~PAGuiHPP2Window();

private:
};

class PAGuiHPP3Window : public PAGuiWindow {
public:
    PAGuiHPP3Window();
    ~PAGuiHPP3Window();

private:
};

class PAGuiHPP4Window : public PAGuiWindow {
public:
    PAGuiHPP4Window();
    ~PAGuiHPP4Window();

private:
};

class PAGuiFuelP1Window : public PAGuiWindow {
public:
    PAGuiFuelP1Window();
    ~PAGuiFuelP1Window();

private:
};

class PAGuiFuelP2Window : public PAGuiWindow {
public:
    PAGuiFuelP2Window();
    ~PAGuiFuelP2Window();

private:
};

class PAGuiFuelP3Window : public PAGuiWindow {
public:
    PAGuiFuelP3Window();
    ~PAGuiFuelP3Window();

private:
};

class PAGuiFuelP4Window : public PAGuiWindow {
public:
    PAGuiFuelP4Window();
    ~PAGuiFuelP4Window();

private:
};

class PAGuiBarWindow : public PAGuiWindow {
public:
	PAGuiBarWindow();
	~PAGuiBarWindow();

private:
};

class PAGuiPauseWindow : public PAGuiWindow {
public:
  PAGuiPauseWindow();
  ~PAGuiPauseWindow();

private:
};

class PAGuiLogoWindow : public PAGuiWindow {
public:
  PAGuiLogoWindow();
  ~PAGuiLogoWindow();

private:
};

class PAGuiFinishWindow : public PAGuiWindow {
public:
    PAGuiFinishWindow();
    ~PAGuiFinishWindow();

private:
};

class PAGuiConfirmWindow : public PAGuiWindow {
public:
    PAGuiConfirmWindow();
    ~PAGuiConfirmWindow();

private:
};

class PAGuiConnectWindow : public PAGuiWindow {
public:
    PAGuiConnectWindow();
    ~PAGuiConnectWindow();

private:
};

typedef struct ObjectC
{

}ObjectC;

extern PAGuiTreeNode* tn_object;