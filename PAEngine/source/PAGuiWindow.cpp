#include "pch.h"

#include "PAGuiWindow.h"
#include "imgui.h"
#include "App.h"
#include "PlayerWin.h"
#include "../../PARender/inc/PARenderPCH.h"

PAGuiTreeNode* tn_object;

PAGuiWindow::PAGuiWindow(std::string windowTitle)
  : title_(windowTitle) {
}

PAGuiWindow::~PAGuiWindow(){
    for (PAGuiWidgets* w : widgets_) {
        delete w;
    }
}

void PAGuiWindow::Update() {
  int width = Utility::GetWindowWidth();
  int height = Utility::GetWindowHeight();

  if (title_ == "Menu Window")
  {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "Connect")
  {
      int neww = width / 4;
      int newh = height / 4;
      ImGui::SetNextWindowPos(ImVec2(width / 2 - neww / 2, height / 2 - newh / 2));
      ImGui::SetNextWindowSize(ImVec2(neww, newh));

      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "Win")
  {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(width, height));
      ImGui::SetNextWindowBgAlpha(0.0f);
      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "Pause Window")
  {
      int neww = width / 6;
      int newh = height / 2;
      ImGui::SetNextWindowPos(ImVec2(width / 2 - neww / 2, height / 2 - newh / 2));
      ImGui::SetNextWindowSize(ImVec2(neww, newh));

      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
  }
  else if (title_ == "Press Window")
  {
      int neww = width / 2;
      int newh = neww / 10;
      ImGui::SetNextWindowPos(ImVec2(width / 2 - neww / 2, height * 3 / 4 - newh));
      ImGui::SetNextWindowSize(ImVec2(neww, newh));
      ImGui::SetNextWindowBgAlpha(0.0f);

      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "Confirm")
  {
      int neww = width / 4;
      int newh = height / 4;
      ImGui::SetNextWindowPos(ImVec2(width / 2 - neww / 2, height / 2 - newh / 2));
      ImGui::SetNextWindowSize(ImVec2(neww, newh));
      ImGui::SetNextWindowBgAlpha(1.0f);

      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "UI")
  {
	  int neww = width * 200 / 1280;
	  int newh = width * 75 / 720;
	  ImGui::SetNextWindowPos(ImVec2(width / 2 - neww / 2, 0));
	  ImGui::SetNextWindowSize(ImVec2(neww, newh));
	  ImGui::SetNextWindowBgAlpha(0.0f);
	  ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "Bar")
  {
	  int neww = width / 2;
	  int newh = neww / 10;
	  ImGui::SetNextWindowPos(ImVec2(width / 2 - neww / 2, height - newh));
	  ImGui::SetNextWindowSize(ImVec2(neww, newh));
	  ImGui::SetNextWindowBgAlpha(0.0f);
	  ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "Logo")
  {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "Finish")
  {
    ImGui::SetNextWindowPos(ImVec2(-10, -10));
    ImGui::SetNextWindowSize(ImVec2(width + 20, height + 20));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "HP1")
  {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(width, height));
      ImGui::SetNextWindowBgAlpha(0.0f);
      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "HP2")
  {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(width, height));
      ImGui::SetNextWindowBgAlpha(0.0f);
      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "HP3")
  {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(width, height));
      ImGui::SetNextWindowBgAlpha(0.0f);
      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "HP4")
  {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(width, height));
      ImGui::SetNextWindowBgAlpha(0.0f);
      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "Fuel1")
  {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(width, height));
      ImGui::SetNextWindowBgAlpha(0.0f);
      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "Fuel2")
  {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(width, height));
      ImGui::SetNextWindowBgAlpha(0.0f);
      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "Fuel3")
  {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(width, height));
      ImGui::SetNextWindowBgAlpha(0.0f);
      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else if (title_ == "Fuel4")
  {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(width, height));
      ImGui::SetNextWindowBgAlpha(0.0f);
      ImGui::Begin(title_.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  }
  else
  {
    ImGui::Begin(title_.c_str());
  }

  for (PAGuiWidgets* w : widgets_) {
      w->update();
  }
  ImGui::End();
}

void PAGuiWindow::AddWidget(PAGuiWidgets* widget) {
    widgets_.push_back(widget);
}

static void Wrap_RestartScene()
{
  PAEngine::App::globalApp->RestartScene();
}
static void Wrap_WinRestartScene()
{
  PAEngine::App::globalApp->RestartScene();
  PlayerWin::globalWin->SetWinner(-1);
  EventSystem::SendEvent(ET_WINONOFF, (void*)nullptr, sizeof(nullptr));
}
static void Wrap_ChangeScene(std::string name)
{
  PAEngine::App::globalApp->ChangeScene(name);
}


static void Wrap_Play()
{
  PAEngine::App::globalApp->ChangeScene("castle");
  EventSystem::SendEvent(ET_UIONOFF, (void*)nullptr, sizeof(nullptr));
}

static void Wrap_Resume()
{
  EventSystem::SendEvent(ET_PAUSEONOFF, (void*)nullptr, sizeof(nullptr));
  EventSystem::SendEvent(ET_UIONOFF, (void*)nullptr, sizeof(nullptr));
}

static void Wrap_Menu()
{
  EventSystem::SendEvent(ET_PAUSEONOFF, (void*)nullptr, sizeof(nullptr));
  //PAEngine::App::globalApp->ChangeScene("menu");
  EventSystem::SendEvent(ET_UIONOFF, (void*)nullptr, sizeof(nullptr));
}


static void Wrap_Quit()
{
  Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "ET_INPUT_QUIT");
  PostMessage(PAEngine::App::globalApp->m_pAppWindow->GetMainWindowHandle(), WM_CLOSE, 0, 0);
}

PAGuiTestWindow::PAGuiTestWindow()
  : PAGuiWindow("TestTitle") {

  PAGuiCollapsingHeader* buttonHeader = new PAGuiCollapsingHeader("BUTTON HEADER");

  PAGuiTreeNode* tn_Scene = new PAGuiTreeNode("Scene Button");
  tn_Scene->AddWidget(new PAGuiButton("RestartScene", &Wrap_RestartScene));
  tn_Scene->AddWidget(new PAGuiLoading("ChangeScene", &Wrap_ChangeScene));
  tn_Scene->AddWidget(new PAGuiText("Scence Button"));
  tn_Scene->AddWidget(new PAGuiDirectLoading("Test", &Wrap_ChangeScene));
  tn_Scene->AddWidget(new PAGuiDirectLoading("player", &Wrap_ChangeScene));
  buttonHeader->AddWidget(tn_Scene);

  AddWidget(buttonHeader);


  PAGuiCollapsingHeader* objectHeader = new PAGuiCollapsingHeader("OBJECT HEADER");

  tn_object = new PAGuiTreeNode("Object Manager");
  tn_object->AddWidget(new PAGuiCreateObj("Create Object"));
  tn_object->AddWidget(new PAGuiDeleteObj("Delete Object"));
  objectHeader->AddWidget(tn_object);

  AddWidget(objectHeader);


}

PAGuiTestWindow::~PAGuiTestWindow(){
}

PAGuiFPSWindow::PAGuiFPSWindow()
  : PAGuiWindow("FPS Window")
{
  AddWidget(new PAGuiFPS("FPS"));
  AddWidget(new PAGuiText("StopWatch"));
  AddWidget(new PAGuiStopwatch());
}

PAGuiFPSWindow::~PAGuiFPSWindow() {
}

PAGuiPressWindow::PAGuiPressWindow()
  : PAGuiWindow("Press Window")
{
  AddWidget(new PAGuiPress());
}

PAGuiPressWindow::~PAGuiPressWindow() {
}
PAGuiMenuWindow::PAGuiMenuWindow()
  : PAGuiWindow("Menu Window")
{
  AddWidget(new PAGuiNothing(&Wrap_Play, &Wrap_Quit));
}

PAGuiMenuWindow::~PAGuiMenuWindow() {
}

PAGuiPauseWindow::PAGuiPauseWindow()
  : PAGuiWindow("Pause Window")
{
  AddWidget(new PAGuiPause(&Wrap_Resume, &Wrap_Menu, &Wrap_Quit));
}

PAGuiPauseWindow::~PAGuiPauseWindow() {
}

PAGuiWinWindow::PAGuiWinWindow()
  : PAGuiWindow("Win")
{
  AddWidget(new PAGuiWinner());
}
PAGuiWinWindow::~PAGuiWinWindow()
{

}
PAGuiLogoWindow::PAGuiLogoWindow()
  : PAGuiWindow("Logo")
{
  AddWidget(new PAGuiLogo());
}
PAGuiLogoWindow::~PAGuiLogoWindow()
{

}

PAGuiUIWindow::PAGuiUIWindow()
  : PAGuiWindow("UI")
{
  AddWidget(new PAGuiTimer());
}
PAGuiUIWindow::~PAGuiUIWindow()
{

}

PAGuiHPP1Window::PAGuiHPP1Window()
  : PAGuiWindow("HP1")
{
  AddWidget(new PAGuiHPBar(0));
}
PAGuiHPP1Window::~PAGuiHPP1Window()
{

}

PAGuiHPP2Window::PAGuiHPP2Window()
  : PAGuiWindow("HP2")
{
  AddWidget(new PAGuiHPBar(1));
}
PAGuiHPP2Window::~PAGuiHPP2Window()
{

}
PAGuiHPP3Window::PAGuiHPP3Window()
  : PAGuiWindow("HP3")
{
  AddWidget(new PAGuiHPBar(2));
}
PAGuiHPP3Window::~PAGuiHPP3Window()
{

}
PAGuiHPP4Window::PAGuiHPP4Window()
  : PAGuiWindow("HP4")
{
  AddWidget(new PAGuiHPBar(3));
}
PAGuiHPP4Window::~PAGuiHPP4Window()
{

}

PAGuiFuelP1Window::PAGuiFuelP1Window()
    : PAGuiWindow("Fuel1")
{
    AddWidget(new PAGuiFuelBar(0));
}
PAGuiFuelP1Window::~PAGuiFuelP1Window()
{

}

PAGuiFuelP2Window::PAGuiFuelP2Window()
    : PAGuiWindow("Fuel2")
{
    AddWidget(new PAGuiFuelBar(1));
}
PAGuiFuelP2Window::~PAGuiFuelP2Window()
{

}
PAGuiFuelP3Window::PAGuiFuelP3Window()
    : PAGuiWindow("Fuel3")
{
    AddWidget(new PAGuiFuelBar(2));
}
PAGuiFuelP3Window::~PAGuiFuelP3Window()
{

}
PAGuiFuelP4Window::PAGuiFuelP4Window()
    : PAGuiWindow("Fuel4")
{
    AddWidget(new PAGuiFuelBar(3));
}
PAGuiFuelP4Window::~PAGuiFuelP4Window()
{

}

PAGuiBarWindow::PAGuiBarWindow()
  : PAGuiWindow("Bar")
{
  AddWidget(new PAGuiBar());
}
PAGuiBarWindow::~PAGuiBarWindow()
{

}

PAGuiFinishWindow::PAGuiFinishWindow()
    : PAGuiWindow("Finish")
{
    AddWidget(new PAGuiFinish());
}
PAGuiFinishWindow::~PAGuiFinishWindow()
{

}
PAGuiConfirmWindow::PAGuiConfirmWindow()
    : PAGuiWindow("Confirm")
{
    AddWidget(new PAGuiConfirm());
}
PAGuiConfirmWindow::~PAGuiConfirmWindow()
{

}

PAGuiConnectWindow::PAGuiConnectWindow()
    : PAGuiWindow("Connect")
{
    AddWidget(new PAGuiConnect());
}
PAGuiConnectWindow::~PAGuiConnectWindow()
{

}