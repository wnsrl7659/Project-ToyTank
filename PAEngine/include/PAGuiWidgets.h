/******************************************************************************
  * Author : Junki Kim
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once
#include <string> // text, checkbox, headers
#include <vector> // collapsing header
#include <queue>  // FPS


class PAGuiWidgets {
public:
    PAGuiWidgets() {}
    virtual ~PAGuiWidgets() {}
    virtual void update() = 0;
};

class PAGuiText : public PAGuiWidgets {
public:
    PAGuiText(std::string text);
    ~PAGuiText();

    void update();
private:
    std::string text_;
};

class PAGuiCheckBox : public PAGuiWidgets {
public:
    PAGuiCheckBox(std::string name, bool* target);
    ~PAGuiCheckBox();

    void update();
private:
    std::string name_;
    bool* target_;
};

class PAGuiButton : public PAGuiWidgets {
public:
    PAGuiButton(std::string name, void (*pfun)(), int width = 0, int height = 0);
    ~PAGuiButton();

    void update();
private:
    std::string name_;
    int width_, height_;
    void (*fun_ptr)();
};

class PAGuiSlider : public PAGuiWidgets {
public:
    PAGuiSlider(std::string name, int* target, int min, int max);
    ~PAGuiSlider();

    void update();
private:
    std::string name_;
    int* target_;
    int min_;
    int max_;
};

class PAGuiDragInt : public PAGuiWidgets {
public:
    PAGuiDragInt(std::string name, int* target);
    ~PAGuiDragInt();

    void update();
private:
    std::string name_;
    int* target_;
};

class PAGuiCollapsingHeader : public PAGuiWidgets {
public:
    PAGuiCollapsingHeader(std::string name);
    ~PAGuiCollapsingHeader();

    void update();

    void AddWidget(PAGuiWidgets* widget);
private:
    std::string name_;
    std::vector<PAGuiWidgets*> widgets_;
};

class PAGuiTreeNode : public PAGuiWidgets {
public:
    PAGuiTreeNode(std::string name);
    ~PAGuiTreeNode();

    void update();

    void AddWidget(PAGuiWidgets* widget);
    void DeleteWidget(int oid);
private:
    std::string name_;
    std::vector<PAGuiWidgets*> widgets_;
};

class PAGuiTestCustomWidget : public PAGuiWidgets {
public:
    PAGuiTestCustomWidget();
    ~PAGuiTestCustomWidget();

    void update();
private:
    int a, b, c;
    float f;
    bool yes;

};

class PAGuiLoading : public PAGuiWidgets {
public:
  PAGuiLoading(std::string button_name, void (*pfun)(std::string));
  ~PAGuiLoading();

  void update();
private:
  std::string button_name_;
  void (*fun_ptr)(std::string);
};

class PAGuiDirectLoading : public PAGuiWidgets {
public:
  PAGuiDirectLoading(std::string button_name, void (*pfun)(std::string));
  ~PAGuiDirectLoading();

  void update();
private:
  std::string button_name_;
  void (*fun_ptr)(std::string);
};


class PAGuiCreateObj : public PAGuiWidgets {
public:
  PAGuiCreateObj(std::string name);
  ~PAGuiCreateObj();

  void update();
private:
  std::string name_;
  bool trans, model, behav;
  float Xscale, Yscale, Zscale, mass, Xpos, Ypos, Zpos;
};

class PAGuiDeleteObj : public PAGuiWidgets {
public:
  PAGuiDeleteObj(std::string name);
  ~PAGuiDeleteObj();

  void update();
private:
  std::string name_;
  int id;
};


class PAGuiTransform : public PAGuiWidgets {
public:
  PAGuiTransform(std::string name, int objectid);
  ~PAGuiTransform();

  void update();
  int oid;
private:
  std::string name_;
  float Xpos, Ypos, Zpos;
};

class PAGuiFPS : public PAGuiWidgets {
public:
    PAGuiFPS(std::string name);
    ~PAGuiFPS();

    void update();
private:
    std::string name_;
    std::queue<float> FPSqueue;
};

class PAGuiStopwatch : public PAGuiWidgets {
public:
  PAGuiStopwatch();
  ~PAGuiStopwatch();

  void update();
private:
  std::string name_; 
};

class PAGuiWinner : public PAGuiWidgets {
public:
  PAGuiWinner();
  ~PAGuiWinner();

  void update();
private:
};

class PAGuiNothing : public PAGuiWidgets {
public:
  PAGuiNothing(void (*pfun1)(), void (*pfun2)());
  ~PAGuiNothing();

  void update();
private:
  void (*fun_ptr1)();
  void (*fun_ptr2)();
};

class PAGuiPause : public PAGuiWidgets {
public:
  PAGuiPause(void (*pfun1)(), void (*pfun2)(), void (*pfun3)());
  ~PAGuiPause();

  void update();
private:
  void (*fun_ptr1)();
  void (*fun_ptr2)();
  void (*fun_ptr3)();
};

class PAGuiTimer : public PAGuiWidgets {
public:
  PAGuiTimer();
  ~PAGuiTimer();

  void update();
private:
};

class PAGuiLogo : public PAGuiWidgets {
public:
  PAGuiLogo();
  ~PAGuiLogo();

  void update();
private:
};

class PAGuiBar : public PAGuiWidgets {
public:
  PAGuiBar();
  ~PAGuiBar();

  void update();
private:
};

class PAGuiPress : public PAGuiWidgets {
public:
  PAGuiPress();
  ~PAGuiPress();

  void update();
private:
};

class PAGuiHPBar : public PAGuiWidgets {
public:
    PAGuiHPBar(int id);
    ~PAGuiHPBar();

    void update();
private:
    int id_;
};

class PAGuiFuelBar : public PAGuiWidgets {
public:
    PAGuiFuelBar(int id);
    ~PAGuiFuelBar();

    void update();
private:
    int id_;
};

class PAGuiFinish : public PAGuiWidgets {
public:
    PAGuiFinish();
    ~PAGuiFinish();

    void update();
private:
    float alpha = 0.0f;
    int timer = 0;
};

class PAGuiConfirm : public PAGuiWidgets {
public:
    PAGuiConfirm();
    ~PAGuiConfirm();

    void update();
private:
};

class PAGuiConnect : public PAGuiWidgets {
public:
    PAGuiConnect();
    ~PAGuiConnect();

    void update();
private:
};


extern int GUIwinner;
extern int menutab;
extern int menustate;
extern int pausetab;
extern int confirmtab;
extern int wintab; 
extern int oldcolor[4];