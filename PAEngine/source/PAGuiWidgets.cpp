#include "pch.h"

#include "PAGuiWidgets.h"
#include "imgui.h"
#include "Transform.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "ModelManager.h"
#include "BehaviorManager.h"
#include "ChildTransform.h"
#include "App.h"
#include "FlagBehavior.h"
#include "PlayerBehavior.h"
#include "PlayerController.h"
#include "ServiceLocator.h"
#include "../../PARender/inc/PARenderPCH.h"

static bool changed = false;

int GUIwinner = -1;
int menutab = 0;
int menustate = 0;
int pausetab = 0;
int wintab = 0;
int confirmtab = 0;
//////////////////////////////////////////
// Text
PAGuiText::PAGuiText(std::string text)
  : text_(text) {
}

PAGuiText::~PAGuiText(){
}

void PAGuiText::update() {
    ImGui::Text(text_.c_str());
}
// End Text
///////////////////////////////////////////

///////////////////////////////////////////
// CheckBox
PAGuiCheckBox::PAGuiCheckBox(std::string name, bool* target)
    : name_(name), target_(target) {
}

PAGuiCheckBox::~PAGuiCheckBox() {
}

void PAGuiCheckBox::update() {
    ImGui::Checkbox(name_.c_str(), target_);
}
//End CheckBox
///////////////////////////////////////////

///////////////////////////////////////////
// Button
PAGuiButton::PAGuiButton(std::string name, void (*pfun)(), int width, int height)
    : name_(name), fun_ptr(pfun), width_(width), height_(height) {
}

PAGuiButton::~PAGuiButton() {
}

void PAGuiButton::update() {
    if (ImGui::Button(name_.c_str(), ImVec2(width_, height_)))
    {
        (*fun_ptr)();
    }
}
//End Button
///////////////////////////////////////////

///////////////////////////////////////////
// Slider
PAGuiSlider::PAGuiSlider(std::string name, int* target, int min, int max)
    : name_(name), target_(target), min_(min), max_(max) {
}

PAGuiSlider::~PAGuiSlider() {
}

void PAGuiSlider::update() {
    ImGui::SliderInt(name_.c_str(), target_, min_, max_);
}
//End Slider
///////////////////////////////////////////

///////////////////////////////////////////
// DragInt
PAGuiDragInt::PAGuiDragInt(std::string name, int* target)
    : name_(name), target_(target) {
}

PAGuiDragInt::~PAGuiDragInt() {
}

void PAGuiDragInt::update() {
    ImGui::DragInt(name_.c_str(), target_);
}
//End DragInt
///////////////////////////////////////////

///////////////////////////////////////////
// Collapsing Header

PAGuiCollapsingHeader::PAGuiCollapsingHeader(std::string name)
  : name_(name) {
}

PAGuiCollapsingHeader::~PAGuiCollapsingHeader() {
    for (PAGuiWidgets* w : widgets_) {
        delete w;
    }
}

void PAGuiCollapsingHeader::update() {
    if (ImGui::CollapsingHeader(name_.c_str())) {
        for (PAGuiWidgets* w : widgets_) {
            w->update();
        }
    }
}

void PAGuiCollapsingHeader::AddWidget(PAGuiWidgets* widget) {
    widgets_.push_back(widget);
}

// Collapsing Header
///////////////////////////////////////////

///////////////////////////////////////////
// TreeNode

PAGuiTreeNode::PAGuiTreeNode(std::string name)
    : name_(name) {
}

PAGuiTreeNode::~PAGuiTreeNode() {
    for (PAGuiWidgets* w : widgets_) {
        delete w;
    }
}

void PAGuiTreeNode::update() {
    if (ImGui::TreeNode(name_.c_str())) {
        for (PAGuiWidgets* w : widgets_) {
            w->update();
            if (changed == true)
            {
              changed = false;
              break;
            }
        }
        ImGui::TreePop();
    }
}

void PAGuiTreeNode::AddWidget(PAGuiWidgets* widget) {
    widgets_.push_back(widget);
}
void PAGuiTreeNode::DeleteWidget(int oid)
{
  int i = 0;
  for (PAGuiWidgets* w : widgets_) {
    PAGuiTransform* temp = reinterpret_cast<PAGuiTransform*>(w);
    if (temp->oid == oid)
    {
      delete w;
      widgets_.erase(widgets_.begin() + i);
    }
    i++;
  }
}

// TreeNode
///////////////////////////////////////////

PAGuiTestCustomWidget::PAGuiTestCustomWidget(){
    a = 0;
    b = 0;
    c = 0;
    f = 0.0f;
    yes = true;
}

PAGuiTestCustomWidget::~PAGuiTestCustomWidget(){
}

void PAGuiTestCustomWidget::update(){
    ImGui::Text("test numbers: %d, %d, %d", a, b, c);
    ImGui::DragInt("a", &a);
    ImGui::DragInt("b", &b);
    ImGui::DragInt("c", &c);
    ImGui::Checkbox("yes", &yes);
}
///////////////////////////////////////////

PAGuiLoading::PAGuiLoading(std::string button_name, void (*pfun)(std::string))
  : button_name_(button_name), fun_ptr(pfun) {
}

PAGuiLoading::~PAGuiLoading() {
}

void PAGuiLoading::update() {
  static char NameBuffer[30] = "type scene name here";
  ImGui::InputText("SceneName", NameBuffer, 30);
  std::string Name = NameBuffer;

  if (ImGui::Button(button_name_.c_str()))
  {
    (*fun_ptr)(Name.c_str());
  }
}
///////////////////////////////////////////

///////////////////////////////////////////

PAGuiDirectLoading::PAGuiDirectLoading(std::string button_name, void (*pfun)(std::string))
  : button_name_(button_name), fun_ptr(pfun) {
}

PAGuiDirectLoading::~PAGuiDirectLoading() {
}

void PAGuiDirectLoading::update() {
  if (ImGui::Button(button_name_.c_str()))
  {
    (*fun_ptr)(button_name_.c_str());
  }
}
///////////////////////////////////////////

///////////////////////////////////////////

PAGuiCreateObj::PAGuiCreateObj(std::string name)
  : name_(name) {
  trans = true;
  model = true;
  behav = false;

  Xscale = 1.0f;
  Yscale = 1.0f;
  Zscale = 1.0f;
  mass = 1.0f;
  Xpos = 0.0f;
  Ypos = 0.0f;
  Zpos = 0.0f;
}

PAGuiCreateObj::~PAGuiCreateObj() {
}

void PAGuiCreateObj::update() {
  if (ImGui::TreeNode(name_.c_str()))
  {
    ImGui::Checkbox("CT_TRANSFORM", &trans);
    ImGui::Checkbox("CT_MODEL", &model);
    ImGui::Checkbox("CT_BEHAVIOR", &behav);

    ImGui::DragFloat("Xscale", &Xscale);
    ImGui::DragFloat("Yscale", &Yscale);
    ImGui::DragFloat("Zscale", &Zscale);
    ImGui::DragFloat("mass", &mass);
    ImGui::DragFloat("Xpos", &Xpos);
    ImGui::DragFloat("Ypos", &Ypos);
    ImGui::DragFloat("Zpos", &Zpos);

    if (ImGui::Button(name_.c_str()))
    {
      changed = true;
      int objectid = -1;

      objectid = ObjectManager::GetInstance()->Allocate();
      if (objectid >= 0) {
        Physics* physics = ObjectManager::GetObjectByID(objectid)->GetComponent<Physics>();
        ModelComponent* tankModel = ObjectManager::GetObjectByID(objectid)->GetComponent<ModelComponent>();

        GameObject* obj = dynamic_cast<GameObject*>(ObjectManager::GetInstance()->Get(objectid));
        obj->AddComponent(physics);
        obj->AddComponent(tankModel);

        physics->Create(objectid, Xscale, Yscale, Zscale, mass, Xpos, Ypos, Zpos);
        tankModel->Create(objectid, MT_CUBE_METALGRID);
      }
    }
    ImGui::TreePop();
  }
}
///////////////////////////////////////////

///////////////////////////////////////////

PAGuiDeleteObj::PAGuiDeleteObj(std::string name)
  : name_(name), id(-1){
}

PAGuiDeleteObj::~PAGuiDeleteObj() {
}

void PAGuiDeleteObj::update() {
  if (ImGui::TreeNode(name_.c_str()))
  {
    ImGui::InputInt("Object ID", &id);

    if (ImGui::Button(name_.c_str()))
    {
      changed = true;
      ObjectManager::DestroyObject(id);
    }
    ImGui::TreePop();
  }
}
///////////////////////////////////////////

///////////////////////////////////////////

PAGuiTransform::PAGuiTransform(std::string name, int objectid)
  : name_(name), oid(objectid){
  Xpos = 0.0f;
  Ypos = 0.0f;
  Zpos = 0.0f;
}

PAGuiTransform::~PAGuiTransform() {
}

void PAGuiTransform::update() {
  if (ImGui::TreeNode(name_.c_str()))
  {
   ;

    if (ObjectManager::GetObjectByID(oid)->GetParent() != -1)
    {
      ImGui::Text("Object Type : Child");
      Transform* transform = ObjectManager::GetObjectByID(oid)->GetComponent<Transform>();
      const float* matrix = transform->GetTransform();

      ImGui::Text("Transform Position : (%f, %f, %f)", matrix[12], matrix[13], matrix[14]);
    }
    else
    {
      ImGui::Text("Object Type : Parent");
      Physics* transform = ObjectManager::GetObjectByID(oid)->GetComponent<Physics>();
      std::vector<float>& matrix = transform->GetMatrix();

      ImGui::Text("Transform Position : (%f, %f, %f)", matrix[12], matrix[13], matrix[14]);

      ImGui::DragFloat("Xpos", &Xpos);
      ImGui::DragFloat("Ypos", &Ypos);
      ImGui::DragFloat("Zpos", &Zpos);

      if (ImGui::Button("Change Transform"))
      {
        transform->Teleport(Xpos, Ypos, Zpos);
      }

      if (ImGui::Button("Reset to Zero"))
      {
        transform->Teleport(-matrix[12], -matrix[13], -matrix[14]);
      }
    }

    ImGui::TreePop();
  }
}
///////////////////////////////////////////

PAGuiFPS::PAGuiFPS(std::string name)
    : name_(name){
}

PAGuiFPS::~PAGuiFPS() {
}

void PAGuiFPS::update() {

    float fps = ImGui::GetIO().Framerate;

    FPSqueue.push(fps);

    int extraSpace = FPSqueue.size() - 120;
    for (int i = 0; i < extraSpace; i++)
        FPSqueue.pop();

    std::queue<float> duplicate = FPSqueue;

    float values[121];
    values[0] = 0;
    int queueSize = duplicate.size();
    for (int i = 0; i < queueSize; i++)
    {
        values[i + 1] = duplicate.front();
        duplicate.pop();
    }

    ImGui::PlotLines("Graph", values, queueSize, 1);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
///////////////////////////////////////////

PAGuiStopwatch::PAGuiStopwatch()
{
}

PAGuiStopwatch::~PAGuiStopwatch()
{
}

void PAGuiStopwatch::update()
{
  std::vector<std::string> names =
  {
    "m_pSceneManager\t",
    "objectManager\t  ",
    "EnvironmentManager ",
    "Physics\t\t\t",
    "Camera\t\t\t ",
    "Render\t\t\t ",
  };
  const std::vector<float> fvector_ = GetFPStimes();
  std::ostringstream oss;
  for (unsigned int i = 0; i < fvector_.size(); i++)
  {
    if (i >= names.size())
      break;
    oss << names[i] << "\t: " << unsigned(fvector_[i]) << "\n";
  }
  ImGui::Text(oss.str().c_str());
}

///

PAGuiWinner::PAGuiWinner()
{

}

PAGuiWinner::~PAGuiWinner()
{

}

void PAGuiWinner::update()
{
  int width = Utility::GetWindowWidth();
  int height = Utility::GetWindowHeight();
  int titlewidth = width / 2;
  int titleheight = height / 10;

  ImGui::Dummy(ImVec2(0, height / 40));
  ImGui::Dummy(ImVec2(width / 4, 0));
  ImGui::SameLine();
  ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/winner.png"), ImVec2(titlewidth, titleheight));

  ImGui::Dummy(ImVec2(0, height / 10));
  ImGui::Dummy(ImVec2(width * 3 / 8, 0));
  ImGui::SameLine();
  switch (GUIwinner)
  {
  case 0:
      ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/tank1_c.png"), ImVec2(width / 4, width / 4));
    break;
  case 1:
      ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/tank2_c.png"), ImVec2(width / 4, width / 4));
    break;
  case 2:
      ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/tank3_c.png"), ImVec2(width / 4, width / 4));
    break;
  case 3:
      ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/tank4_c.png"), ImVec2(width / 4, width / 4));
    break;
  }

  ImGui::Dummy(ImVec2(0, height / 10));
  if (wintab == 0)
  {
      ImGui::Dummy(ImVec2(width / 5, 0));
      ImGui::SameLine();
      ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/Rselect.png"), ImVec2(width / 5, height / 10));
      ImGui::SameLine();
      ImGui::Dummy(ImVec2(width / 5, 0));
      ImGui::SameLine();
      ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/Qproto.png"), ImVec2(width / 5, height / 10));
  }
  else
  {
      ImGui::Dummy(ImVec2(width / 5, 0));
      ImGui::SameLine();
      ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/Rproto.png"), ImVec2(width / 5, height / 10));
      ImGui::SameLine();
      ImGui::Dummy(ImVec2(width / 5, 0));
      ImGui::SameLine();
      ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/Qselect.png"), ImVec2(width / 5, height / 10));
  }
}



PAGuiNothing::PAGuiNothing(void (*pfun1)(), void (*pfun2)())
  : fun_ptr1(pfun1), fun_ptr2(pfun2)
{

}

PAGuiNothing::~PAGuiNothing()
{

}

void PAGuiNothing::update()
{
  ImGui::SetWindowFontScale(1.3);

  int width = Utility::GetWindowWidth();
  int height = Utility::GetWindowHeight();
  int neww = width / 2;
  int newh = neww / 5;
  ImGui::Dummy(ImVec2(0, newh / 2));
  ImGui::Dummy(ImVec2(neww / 2, 0));
  ImGui::SameLine();
  ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/title.png"), ImVec2(neww, newh - 10));
  //640 * 128

  static const ImVec4 pressColor{ 0.455f, 0.198f, 0.301f, 1.00f };
  static const ImVec4 releaseColor{ 0.47f, 0.77f, 0.83f, 1.00f };
  static const ImVec2 size{ (float)neww / 4.0f, (float)newh / 2.0f };
  if (menustate == 0)
  {
    ImGui::Dummy(ImVec2(0, newh * 3 / 4));

    for (int i = 0; i < 4; i++)
    {
      if (i > 0)
      {
        ImGui::Dummy(ImVec2(0, newh / 6));
      }
      if (i >= 0)
      {
        ImGui::Dummy(ImVec2(neww / 2 + neww * 3 / 8, 0));
        ImGui::SameLine();
      }
      if (menutab == i)
      {
        ImGui::PushStyleColor(ImGuiCol_Button, pressColor);
        ImGui::PushStyleColor(ImGuiCol_Text, releaseColor);
      }
      else
      {
        ImGui::PushStyleColor(ImGuiCol_Button, releaseColor);
        ImGui::PushStyleColor(ImGuiCol_Text, pressColor);
      }

      if (i == 0)
      {
        if (ImGui::Button("Play", size))
        {
          (*fun_ptr1)();
        }
      }
      else if (i == 1)
      {
        if (ImGui::Button("How to Play", size))
        {
        }
      }
      else if (i == 2)
      {
        if (ImGui::Button("Credits", size))
        {
        }
      }
      else if (i == 3)
      {
        if (ImGui::Button("Quit", size))
        {
          (*fun_ptr2)();
        }
      }
      ImGui::PopStyleColor(1);
      ImGui::PopStyleColor(1);
    }
  }
  else if (menustate == 1)
  {
    ImGui::Dummy(ImVec2(0, newh /2));
    ImGui::Dummy(ImVec2(neww / 2, 0));
    ImGui::SameLine();
    ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/unknown.png"), ImVec2(neww, newh * 3.5 - 10));
    //640 x 440
  }
  else if (menustate == 2)
  {
    ImGui::Dummy(ImVec2(0, newh / 2));
    ImGui::Dummy(ImVec2(neww / 2, 0));
    ImGui::SameLine();
    ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/credits.png"), ImVec2(neww, newh * 3.5 - 10));
  }
}

PAGuiTimer::PAGuiTimer()
{
}

PAGuiTimer::~PAGuiTimer() {
}

//static bool ryry = 0;
void PAGuiTimer::update() 
{
	int width = Utility::GetWindowWidth();
	int height = Utility::GetWindowHeight();

  std::vector<GameObject*> flagObj = ObjectManager::GetObjectsByName("flag");
  if (flagObj.size() < 1)
  {
    return;
  }
  FlagBehavior* flag = flagObj[0]->GetComponent<FlagBehavior>();

  ImGui::SetWindowFontScale(width * 4 / 1280);
  int PID = flag->GetPlayer();

  int time = flag->GetWinTime();
  int timei = time / 60.0f;
  float timef = time / 60.0f;
  int timeMin = timei / 60;
  int timeSec = timei - 60 * timeMin;

  if (timeMin > 0)
  {
    ImGui::Dummy(ImVec2(0, width * 8 / 1280));
    ImGui::SameLine();
    if (timeSec > 9)
    {
      ImGui::Text(" %d:%d", timeMin, timeSec);
    }
    else
    {
      ImGui::Text(" %d:0%d", timeMin, timeSec);
    }

  }
  else if (timef >= 10)
  {
	  ImGui::Dummy(ImVec2(width * 8 / 1280, 0));
	  ImGui::SameLine();
    ImGui::Text(" %.1f", timef);
    //ImGui::TextColored(ImVec4(0, 0, 0.0f, 0.0f), " %.1f", timef);
  }
  else if (timef < 0)
  {
	  ImGui::Dummy(ImVec2(width * 8 / 1280, 0));
	  ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0, 0.0f, 1.0f), " %.1f", 0);
  }
  else
  {
    float decimals = timef - (int)timef;
    if (decimals > 0.5f)
    {
	  ImGui::Dummy(ImVec2(width * 8 / 1280, 0));
	  ImGui::SameLine();
      ImGui::TextColored(ImVec4(1.0f, (1.0f - decimals) * 2, 0.0f, 1.0f), " %.1f", timef);
    }
    else
    {
	  ImGui::Dummy(ImVec2(width * 8 / 1280, 0));
	  ImGui::SameLine();
      ImGui::TextColored(ImVec4(1.0f, (decimals) * 2, 0.0f, 1.0f), " %.1f", timef);
    }
  }
  ImGui::SameLine();
    

}

PAGuiPause::PAGuiPause(void (*pfun1)(), void (*pfun2)(), void (*pfun3)())
  : fun_ptr1(pfun1), fun_ptr2(pfun2), fun_ptr3(pfun3)
{

}

PAGuiPause::~PAGuiPause()
{

}

void PAGuiPause::update()
{
  int width = Utility::GetWindowWidth();
  int height = Utility::GetWindowHeight();
  int neww = width / 6 * 4 / 5;
  int newh = height / 2 / 4;

  ImGui::SetWindowFontScale(1.6);

  ImGui::Text("\n");
  ImGui::Dummy(ImVec2((float)width / 6.0f / 20.0f, 0));
  ImGui::SameLine();

  static const ImVec4 pressColor{ 0.455f, 0.198f, 0.301f, 1.00f };
  static const ImVec4 releaseColor{ 0.47f, 0.77f, 0.83f, 0.14f };
  static const ImVec2 size{ (float)neww, (float)newh };

  for (int i = 0; i < 3; i++)
  {
    if (i > 0)
    {
      ImGui::Text("\n");
      ImGui::Dummy(ImVec2((float)width / 6.0f / 20.0f, 0));
      ImGui::SameLine();
    }
    if (pausetab == i)
      ImGui::PushStyleColor(ImGuiCol_Button, pressColor);
    else
      ImGui::PushStyleColor(ImGuiCol_Button, releaseColor);

    if (i == 0)
    {
      if (ImGui::Button("Resume", size))
      {
        pausetab = 0;
        (*fun_ptr1)();
      }
    }
    else if (i == 1)
    {
      if (ImGui::Button("Restart", size))
      {
        pausetab = 1;
        (*fun_ptr2)();
      }
    }
    else if (i == 2)
    {
      if (ImGui::Button("Quit", size))
      {
        pausetab = 2;
        (*fun_ptr3)();
      }
    }
    ImGui::PopStyleColor(1);
  }
}

PAGuiLogo::PAGuiLogo()
{
}

PAGuiLogo::~PAGuiLogo() {
}

void PAGuiLogo::update()
{
  int width = Utility::GetWindowWidth();
  int height = Utility::GetWindowHeight();

  ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/DigiPenLogo.png"), ImVec2(width, height - 10));

}

PAGuiFinish::PAGuiFinish()
{
}

PAGuiFinish::~PAGuiFinish() {
}

void PAGuiFinish::update()
{
    int width = Utility::GetWindowWidth() + 20;
    int height = Utility::GetWindowHeight() + 20;
    int newh = height / 10;

    ImGui::Dummy(ImVec2(0, height / 2 - newh / 2));
    ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/FINISH.png"), ImVec2(width, newh), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, alpha));
    if (alpha < 1.0f)
    {
        alpha += 0.01f;
    }
    else
    {
        timer++;
    }
    if (timer >= 60)
    {
        std::vector<GameObject*> flagObj = ObjectManager::GetObjectsByName("flag");
        if (flagObj.size() < 1)
        {
            return;
        }

        FlagBehavior* flag = flagObj[0]->GetComponent<FlagBehavior>();

        flag->fin3 = true;

        EventSystem::SendEvent(ET_FINISHONOFF, (void*)nullptr, sizeof(nullptr));
        alpha = 0.0f;
        timer = 0;
    }
}

PAGuiConfirm::PAGuiConfirm()
{
}

PAGuiConfirm::~PAGuiConfirm() {
}

void PAGuiConfirm::update()
{
    int width = Utility::GetWindowWidth();
    int height = Utility::GetWindowHeight(); 
    int neww = width / 4;
    int newh = height / 4;

    ImGui::SetWindowFontScale(1.6);

    static const ImVec4 pressColor{ 0.455f, 0.198f, 0.301f, 1.00f };
    static const ImVec4 releaseColor{ 0.47f, 0.77f, 0.83f, 0.14f };
    static const ImVec2 size{ (float)neww * 2.0f / 5.f, (float)newh * 2.0f / 5.f };

    ImGui::Text("\n");
    ImGui::Text("Are you sure you want to quit?");
    ImGui::Text("\n");
    ImGui::Text("\n");

    ImGui::Dummy(ImVec2((float)neww / 180.0f, 0));
    ImGui::SameLine();

    for (int i = 0; i < 3; i++)
    {
        if (i > 0)
        {
            ImGui::SameLine();
            ImGui::Dummy(ImVec2((float)neww / 20.0f, 0)); 
            ImGui::SameLine();
        }
        if (confirmtab == i)
            ImGui::PushStyleColor(ImGuiCol_Button, pressColor);
        else
            ImGui::PushStyleColor(ImGuiCol_Button, releaseColor);

        if (i == 0)
        {
            if (ImGui::Button("Yes", size))
            {
                confirmtab = 0;
               //(*fun_ptr1)();
            }
        }
        else if (i == 1)
        {
            if (ImGui::Button("No", size))
            {
                confirmtab = 1;
                //(*fun_ptr2)();
            }
        }
        ImGui::PopStyleColor(1);
    }
}

int oldcolor[4] = { 0,0,0,0 };
PAGuiBar::PAGuiBar()
{
}

PAGuiBar::~PAGuiBar() {
}

void PAGuiBar::update()
{
	int width = Utility::GetWindowWidth();
	int height = Utility::GetWindowHeight();
	int neww = width / 2 * 0.975;
	int newh = neww / 10 * 0.9;

	std::vector<GameObject*> flagObj = ObjectManager::GetObjectsByName("flag");
	if (flagObj.size() < 1)
	{
		return;
	}

	FlagBehavior* flag = flagObj[0]->GetComponent<FlagBehavior>();

	float min = 9.0f;

	if (flag->redPercent > 1.0f)
	{
		ImGui::SameLine();
        if (oldcolor[0] % 20 <= 10)
        {
            ImGui::ColorButton("", ImVec4(1, 0, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(neww * flag->redPercent / 100 - min, newh));
        }
        else
        {
            ImGui::ColorButton("", ImVec4(0.5, 0, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(neww * flag->redPercent / 100 - min, newh));
        }
        if (oldcolor[0] > 0)
        {
            oldcolor[0]--;
        }
	}
	if (flag->bluePercent > 1.0f)
	{
		ImGui::SameLine();
        if (oldcolor[1] % 20 <= 10)
        {
            ImGui::ColorButton("", ImVec4(0, 0, 1, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(neww * flag->bluePercent / 100 - min, newh));
        }
        else
        {
            ImGui::ColorButton("", ImVec4(0, 0, 0.5, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(neww * flag->bluePercent / 100 - min, newh));
        }
        if (oldcolor[1] > 0)
        {
            oldcolor[1]--;
        }
	}
	if (flag->greenPercent > 1.0f)
	{
		ImGui::SameLine();
        if (oldcolor[2] % 20 <= 10)
        {
            ImGui::ColorButton("", ImVec4(0, 1, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(neww * flag->greenPercent / 100 - min, newh));
        }
        else
        {
            ImGui::ColorButton("", ImVec4(0, 0.5, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(neww * flag->greenPercent / 100 - min, newh));
        }
        if (oldcolor[2] > 0)
        {
            oldcolor[2]--;
        }
	}
	if (flag->yellowPercent > 1.0f)
	{
		ImGui::SameLine();
        if (oldcolor[3] % 20 <= 10)
        {
            ImGui::ColorButton("", ImVec4(1, 1, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(neww * flag->yellowPercent / 100 - min, newh));
        }
        else
        {
            ImGui::ColorButton("", ImVec4(0.5, 0.5, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(neww * flag->yellowPercent / 100 - min, newh));
        }
        if (oldcolor[3] > 0)
        {
            oldcolor[3]--;
        }
	}
	if (flag->grayPercent > 1.0f)
	{
		ImGui::SameLine();
		ImGui::ColorButton("", ImVec4(0.5, 0.5, 0.5, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(neww * flag->grayPercent / 100 - min, newh));
	}

}

PAGuiPress::PAGuiPress()
{
}

PAGuiPress::~PAGuiPress() {
}

void PAGuiPress::update()
{
  int width = Utility::GetWindowWidth();
  int height = Utility::GetWindowHeight();
  int neww = width / 2;
  int newh = neww / 10;

  ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/PRESS.png"), ImVec2(neww, newh - 10));

}

static float oldP1 = 0;
static float oldP2 = 0;
static float oldP3 = 0;
static float oldP4 = 0;
static int count = 0;
PAGuiHPBar::PAGuiHPBar(int id)
  : id_(id)
{
}

PAGuiHPBar::~PAGuiHPBar() {
}

void PAGuiHPBar::update()
{
  if (id_ == 0)
  {
    count = 0;
  }
  int width = Utility::GetWindowWidth();
  int height = Utility::GetWindowHeight();
  int newh = height - 10;

  int size_ = Util::ServiceLocator::getGameInput()->GetGamePads().size();

  int objid = PlayerController::GetObjectIDFromPlayerID(id_);
  GameObject* player = ObjectManager::GetObjectByID(objid);
  if (player == NULL)
  {
    return;
  }
  float HP = player->GetComponent<PlayerBehavior>()->HP;
  float lost = 0.0f;

  float changespeed = 0.1f;

  if (size_ == 0 || size_ == 1)
  {
    int newunit = newh / 5;

    if (id_ == 0)
    {
      if (oldP1 != HP)
      {
        if (oldP1 < (HP - changespeed))
        {
          oldP1 += changespeed;
        }
        else if (oldP1 > (HP + changespeed))
        {
          oldP1 -= changespeed;
        }
        if (oldP1 < 0)
        {
          oldP1 = 0;
        }
      }
      lost = 5.0f - oldP1;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
      ImGui::Dummy(ImVec2(0, 0)); //width
      ImGui::SameLine();
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP1 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP1));
    }
    else if (id_ == 1)
    {
      if (oldP2 != HP)
      {
        if (oldP2 < (HP - changespeed))
        {
          oldP2 += changespeed;
        }
        else if (oldP2 > (HP + changespeed))
        {
          oldP2 -= changespeed;
        }
        if (oldP2 < 0)
        {
          oldP2 = 0;
        }
      }
      lost = 5.0f - oldP2;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
      ImGui::Dummy(ImVec2(0, 0)); //width
      ImGui::SameLine();
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP2 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP2));
    }
    else if (id_ == 2)
    {
      if (oldP3 != HP)
      {
        if (oldP3 < (HP - changespeed))
        {
          oldP3 += changespeed;
        }
        else if (oldP3 > (HP + changespeed))
        {
          oldP3 -= changespeed;
        }
        if (oldP3 < 0)
        {
          oldP3 = 0;
        }
      }
      lost = 5.0f - oldP3;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
      ImGui::Dummy(ImVec2(0, 0)); //width
      ImGui::SameLine();
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP3 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP3));
    }
    else if (id_ == 3)
    {
      if (oldP4 != HP)
      {
        if (oldP4 < (HP - changespeed))
        {
          oldP4 += changespeed;
        }
        else if (oldP4 > (HP + changespeed))
        {
          oldP4 -= changespeed;
        }
        if (oldP4 < 0)
        {
          oldP4 = 0;
        }
      }
      lost = 5.0f - oldP4;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
      ImGui::Dummy(ImVec2(0, 0)); //width
      ImGui::SameLine();
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP4 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP4));
    }
  }
  else if (size_ == 2)
  {
    int newunit = newh / 10;
    if (id_ == 0)
    {
      if (oldP1 != HP)
      {
        if (oldP1 < (HP - changespeed))
        {
          oldP1 += changespeed;
        }
        else if (oldP1 > (HP + changespeed))
        {
          oldP1 -= changespeed;
        }
        if (oldP1 < 0)
        {
          oldP1 = 0;
        }
      }
      lost = 5.0f - oldP1;
      ImGui::Dummy(ImVec2(0, (float)newunit* lost)); //height
      ImGui::Dummy(ImVec2(0, 0)); //width
      ImGui::SameLine();
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP1 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP1));
      count++;
    }
    else if (id_ == 1)
    {
      if (oldP2 != HP)
      {
        if (oldP2 < (HP - changespeed))
        {
          oldP2 += changespeed;
        }
        else if (oldP2 > (HP + changespeed))
        {
          oldP2 -= changespeed;
        }
        if (oldP2 < 0)
        {
          oldP2 = 0;
        }
      }
      lost = 5.0f - oldP2;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost + count * (newh / 2))); //height
      ImGui::Dummy(ImVec2(0, 0)); //width
      ImGui::SameLine();
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP2 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP2));
      count++;
    }
    else if (id_ == 2)
    {
      if (oldP3 != HP)
      {
        if (oldP3 < (HP - changespeed))
        {
          oldP3 += changespeed;
        }
        else if (oldP3 > (HP + changespeed))
        {
          oldP3 -= changespeed;
        }
        if (oldP3 < 0)
        {
          oldP3 = 0;
        }
      }
      lost = 5.0f - oldP3;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost + count * (newh / 2))); //height
      ImGui::Dummy(ImVec2(0, 0)); //width
      ImGui::SameLine();
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP3 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP3));
      count++;
    }
    else if (id_ == 3)
    {
      if (oldP4 != HP)
      {
        if (oldP4 < (HP - changespeed))
        {
          oldP4 += changespeed;
        }
        else if (oldP4 > (HP + changespeed))
        {
          oldP4 -= changespeed;
        }
        if (oldP4 < 0)
        {
          oldP4 = 0;
        }
      }
      lost = 5.0f - oldP4;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost + count * (newh / 2))); //height
      ImGui::Dummy(ImVec2(0, 0)); //width
      ImGui::SameLine();
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP4 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP4));
      count++;
    }
  }
  else if (size_ == 3)
  {
    int newunit = newh / 10;
    if (id_ == 0)
    {
      if (oldP1 != HP)
      {
        if (oldP1 < (HP - changespeed))
        {
          oldP1 += changespeed;
        }
        else if (oldP1 > (HP + changespeed))
        {
          oldP1 -= changespeed;
        }
        if (oldP1 < 0)
        {
          oldP1 = 0;
        }
      }
      lost = 5.0f - oldP1;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
      if (count == 0)
      {
        ImGui::Dummy(ImVec2(0.245 * (float)width, 0)); //width
        ImGui::SameLine();
      }
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP1 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP1));
      count++;
    }
    else if (id_ == 1)
    {
      if (oldP2 != HP)
      {
        if (oldP2 < (HP - changespeed))
        {
          oldP2 += changespeed;
        }
        else if (oldP2 > (HP + changespeed))
        {
          oldP2 -= changespeed;
        }
        if (oldP2 < 0)
        {
          oldP2 = 0;
        }
      }
      lost = 5.0f - oldP2;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost + count * (newh / 2))); //height
      if (count == 0)
      {
        ImGui::Dummy(ImVec2(0.25 * width, 0)); //width
        ImGui::SameLine();
      }
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP2 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP2));
      count++;
    }
    else if (id_ == 2)
    {
      if (oldP3 != HP)
      {
        if (oldP3 < (HP - changespeed))
        {
          oldP3 += changespeed;
        }
        else if (oldP3 > (HP + changespeed))
        {
          oldP3 -= changespeed;
        }
        if (oldP3 < 0)
        {
          oldP3 = 0;
        }
      }
      lost = 5.0f - oldP3;
      if (count >= 1)
      {
        ImGui::Dummy(ImVec2(0, (float)newunit * lost + (newh / 2))); //height
      }
      if (count >= 2)
      {
        ImGui::Dummy(ImVec2(width - width / 25, 0)); //width
        ImGui::SameLine();
      }
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP3 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP3));
      count++;
    }
    else if (id_ == 3)
    {
      if (oldP4 != HP)
      {
        if (oldP4 < (HP - changespeed))
        {
          oldP4 += changespeed;
        }
        else if (oldP4 > (HP + changespeed))
        {
          oldP4 -= changespeed;
        }
        if (oldP4 < 0)
        {
          oldP4 = 0;
        }
      }
      lost = 5.0f - oldP4;
      if (count >= 1)
      {
        ImGui::Dummy(ImVec2(0, (float)newunit * lost + (newh / 2))); //height
      }
      if (count >= 2)
      {
        ImGui::Dummy(ImVec2(width - width / 25, 0)); //width
        ImGui::SameLine();
      }
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP4 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP4));
      count++;
    }
  }
  else if(size_ == 4)
  {
    int newunit = newh / 10;
    if (id_ == 0)
    {
      if (oldP1 != HP)
      {
        if (oldP1 < (HP - changespeed))
        {
          oldP1 += changespeed;
        }
        else if (oldP1 > (HP + changespeed))
        {
          oldP1 -= changespeed;
        }
        if (oldP1 < 0)
        {
          oldP1 = 0;
        }
      }
      lost = 5.0f - oldP1;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
      ImGui::Dummy(ImVec2(0, 0)); //width
      ImGui::SameLine();
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP1 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP1));
      count++;
    }
    else if (id_ == 1)
    {
      if (oldP2 != HP)
      {
        if (oldP2 < (HP - changespeed))
        {
          oldP2 += changespeed;
        }
        else if (oldP2 > (HP + changespeed))
        {
          oldP2 -= changespeed;
        }
        if (oldP2 < 0)
        {
          oldP2 = 0;
        }
      }
      lost = 5.0f - oldP2;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
      ImGui::Dummy(ImVec2(width - width / 25, 0)); //width
      ImGui::SameLine();
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP2 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP2));
      count++;
    }
    else if (id_ == 2)
    {
      if (oldP3 != HP)
      {
        if (oldP3 < (HP - changespeed))
        {
          oldP3 += changespeed;
        }
        else if (oldP3 > (HP + changespeed))
        {
          oldP3 -= changespeed;
        }
        if (oldP3 < 0)
        {
          oldP3 = 0;
        }
      }
      lost = 5.0f - oldP3;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost + (newh / 2))); //height
      ImGui::Dummy(ImVec2(0, 0)); //width
      ImGui::SameLine();
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP3 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP3));
      count++;
    }
    else if (id_ == 3)
    {
      if (oldP4 != HP)
      {
        if (oldP4 < (HP - changespeed))
        {
          oldP4 += changespeed;
        }
        else if (oldP4 > (HP + changespeed))
        {
          oldP4 -= changespeed;
        }
        if (oldP4 < 0)
        {
          oldP4 = 0;
        }
      }
      lost = 5.0f - oldP4;
      ImGui::Dummy(ImVec2(0, (float)newunit * lost + (count % 2) * (newh / 2))); //height
      if (count >= 2)
      {
        ImGui::Dummy(ImVec2(width - width / 25, 0)); //width
        ImGui::SameLine();
      }
      ImGui::ColorButton("", ImVec4(lost / 5.0f, oldP4 / 5.0f, 0, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 40, (float)newunit * oldP4));
      count++;
    }
  }
}



static float oldP1f = 0;
static float oldP2f = 0;
static float oldP3f = 0;
static float oldP4f = 0;
static int fuelcount = 0;
PAGuiFuelBar::PAGuiFuelBar(int id)
    : id_(id)
{
}

PAGuiFuelBar::~PAGuiFuelBar() {
}

void PAGuiFuelBar::update()
{
    if (id_ == 0)
    {
        fuelcount = 0;
    }
    int width = Utility::GetWindowWidth();
    int height = Utility::GetWindowHeight();
    int newh = height - 10;

    int size_ = Util::ServiceLocator::getGameInput()->GetGamePads().size();

    int objid = PlayerController::GetObjectIDFromPlayerID(id_);
    GameObject* player = ObjectManager::GetObjectByID(objid);
    if (player == NULL)
    {
        return;
    }
    float fuel = player->GetComponent<PlayerBehavior>()->fuel;
    float lost = 0.0f;

    float changespeed = 1.0f;

    if (size_ == 0 || size_ == 1)
    {
        float newunit = (float)newh / 100.0f;

        if (id_ == 0)
        {
            if (oldP1f != fuel)
            {
                if (oldP1f < (fuel - changespeed))
                {
                    oldP1f += changespeed;
                }
                else if (oldP1f > (fuel + changespeed))
                {
                    oldP1f -= changespeed;
                }
                if (oldP1f < 0)
                {
                    oldP1f = 0;
                }
            }
            lost = 100.0f - oldP1f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
            ImGui::Dummy(ImVec2(width / 40, 0)); //width
            ImGui::SameLine();
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP1f));
        }
        else if (id_ == 1)
        {
            if (oldP2f != fuel)
            {
                if (oldP2f < (fuel - changespeed))
                {
                    oldP2f += changespeed;
                }
                else if (oldP2f > (fuel + changespeed))
                {
                    oldP2f -= changespeed;
                }
                if (oldP2f < 0)
                {
                    oldP2f = 0;
                }
            }
            lost = 100.0f - oldP2f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
            ImGui::Dummy(ImVec2(width / 40, 0)); //width
            ImGui::SameLine();
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP2f));
        }
        else if (id_ == 2)
        {
            if (oldP3f != fuel)
            {
                if (oldP3f < (fuel - changespeed))
                {
                    oldP3f += changespeed;
                }
                else if (oldP3f > (fuel + changespeed))
                {
                    oldP3f -= changespeed;
                }
                if (oldP3f < 0)
                {
                    oldP3f = 0;
                }
            }
            lost = 100.0f - oldP3f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
            ImGui::Dummy(ImVec2(width / 40, 0)); //width
            ImGui::SameLine();
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP3f));
        }
        else if (id_ == 3)
        {
            if (oldP4f != fuel)
            {
                if (oldP4f < (fuel - changespeed))
                {
                    oldP4f += changespeed;
                }
                else if (oldP4f > (fuel + changespeed))
                {
                    oldP4f -= changespeed;
                }
                if (oldP4f < 0)
                {
                    oldP4f = 0;
                }
            }
            lost = 100.0f - oldP4f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
            ImGui::Dummy(ImVec2(width / 40, 0)); //width
            ImGui::SameLine();
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP4f));
        }
    }
    else if (size_ == 2)
    {
        float newunit = (float)newh / 200.0f;
        if (id_ == 0)
        {
            if (oldP1f != fuel)
            {
                if (oldP1f < (fuel - changespeed))
                {
                    oldP1f += changespeed;
                }
                else if (oldP1f > (fuel + changespeed))
                {
                    oldP1f -= changespeed;
                }
                if (oldP1f < 0)
                {
                    oldP1f = 0;
                }
            }
            lost = 100.0f - oldP1f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
            ImGui::Dummy(ImVec2(width / 40, 0)); //width
            ImGui::SameLine();
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP1f));
            fuelcount++;
        }
        else if (id_ == 1)
        {
            if (oldP2f != fuel)
            {
                if (oldP2f < (fuel - changespeed))
                {
                    oldP2f += changespeed;
                }
                else if (oldP2f > (fuel + changespeed))
                {
                    oldP2f -= changespeed;
                }
                if (oldP2f < 0)
                {
                    oldP2f = 0;
                }
            }
            lost = 100.0f - oldP2f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost + fuelcount * (newh / 2))); //height
            ImGui::Dummy(ImVec2(width / 40, 0)); //width
            ImGui::SameLine();
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP2f));
            fuelcount++;
        }
        else if (id_ == 2)
        {
            if (oldP3f != fuel)
            {
                if (oldP3f < (fuel - changespeed))
                {
                    oldP3f += changespeed;
                }
                else if (oldP3f > (fuel + changespeed))
                {
                    oldP3f -= changespeed;
                }
                if (oldP3f < 0)
                {
                    oldP3f = 0;
                }
            }
            lost = 100.0f - oldP3f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost + fuelcount * (newh / 2))); //height
            ImGui::Dummy(ImVec2(width / 40, 0)); //width
            ImGui::SameLine();
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP3f));
            fuelcount++;
        }
        else if (id_ == 3)
        {
            if (oldP4f != fuel)
            {
                if (oldP4f < (fuel - changespeed))
                {
                    oldP4f += changespeed;
                }
                else if (oldP4f > (fuel + changespeed))
                {
                    oldP4f -= changespeed;
                }
                if (oldP4f < 0)
                {
                    oldP4f = 0;
                }
            }
            lost = 100.0f - oldP4f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost + fuelcount * (newh / 2))); //height
            ImGui::Dummy(ImVec2(width / 40, 0)); //width
            ImGui::SameLine();
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP4f));
            fuelcount++;
        }
    }
    else if (size_ == 3)
    {
        float newunit = (float)newh / 200.0f;
        
        if (id_ == 0)
        {
            if (oldP1f != fuel)
            {
                if (oldP1f < (fuel - changespeed))
                {
                    oldP1f += changespeed;
                }
                else if (oldP1f > (fuel + changespeed))
                {
                    oldP1f -= changespeed;
                }
                if (oldP1f < 0)
                {
                    oldP1f = 0;
                }
            }
            lost = 100.0f - oldP1f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
            if (fuelcount == 0)
            {
                ImGui::Dummy(ImVec2(0.245 * (float)width + width / 40, 0)); //width
                ImGui::SameLine();
            }
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP1f));
            fuelcount++;
        }
        else if (id_ == 1)
        {
            if (oldP2f != fuel)
            {
                if (oldP2f < (fuel - changespeed))
                {
                    oldP2f += changespeed;
                }
                else if (oldP2f > (fuel + changespeed))
                {
                    oldP2f -= changespeed;
                }
                if (oldP2f < 0)
                {
                    oldP2f = 0;
                }
            }
            lost = 100.0f - oldP2f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost + fuelcount * (newh / 2))); //height
            if (fuelcount == 0)
            {
                ImGui::Dummy(ImVec2(0.25 * width + width / 40, 0)); //width
                ImGui::SameLine();
            }
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP2f));
            fuelcount++;
        }
        else if (id_ == 2)
        {
            if (oldP3f != fuel)
            {
                if (oldP3f < (fuel - changespeed))
                {
                    oldP3f += changespeed;
                }
                else if (oldP3f > (fuel + changespeed))
                {
                    oldP3f -= changespeed;
                }
                if (oldP3f < 0)
                {
                    oldP3f = 0;
                }
            }
            lost = 100.0f - oldP3f;
            if (fuelcount >= 1)
            {
                ImGui::Dummy(ImVec2(0, (float)newunit * lost + (newh / 2))); //height
            }
            if (fuelcount >= 2)
            {
                ImGui::Dummy(ImVec2(width - width / 25 - width / 120, 0)); //width
                ImGui::SameLine();
            }
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP3f));
            fuelcount++;
        }
        else if (id_ == 3)
        {
            if (oldP4f != fuel)
            {
                if (oldP4f < (fuel - changespeed))
                {
                    oldP4f += changespeed;
                }
                else if (oldP4f > (fuel + changespeed))
                {
                    oldP4f -= changespeed;
                }
                if (oldP4f < 0)
                {
                    oldP4f = 0;
                }
            }
            lost = 100.0f - oldP4f;
            if (fuelcount >= 1)
            {
                ImGui::Dummy(ImVec2(0, (float)newunit * lost + (newh / 2))); //height
            }
            if (fuelcount >= 2)
            {
                ImGui::Dummy(ImVec2(width - width / 25 - width / 120, 0)); //width
                ImGui::SameLine();
            }
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP4f));
            fuelcount++;
        }
    }
    else if (size_ == 4)
    {
        float newunit = (float)newh / 200.0f;
        if (id_ == 0)
        {
            if (oldP1f != fuel)
            {
                if (oldP1f < (fuel - changespeed))
                {
                    oldP1f += changespeed;
                }
                else if (oldP1f > (fuel + changespeed))
                {
                    oldP1f -= changespeed;
                }
                if (oldP1f < 0)
                {
                    oldP1f = 0;
                }
            }
            lost = 100.0f - oldP1f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
            ImGui::Dummy(ImVec2(width / 40, 0)); //width
            ImGui::SameLine();
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP1f));
            fuelcount++;
        }
        else if (id_ == 1)
        {
            if (oldP2f != fuel)
            {
                if (oldP2f < (fuel - changespeed))
                {
                    oldP2f += changespeed;
                }
                else if (oldP2f > (fuel + changespeed))
                {
                    oldP2f -= changespeed;
                }
                if (oldP2f < 0)
                {
                    oldP2f = 0;
                }
            }
            lost = 100.0f - oldP2f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost)); //height
            ImGui::Dummy(ImVec2(width - width / 25 - width / 120, 0)); //width
            ImGui::SameLine();
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP2f));
            fuelcount++;
        }
        else if (id_ == 2)
        {
            if (oldP3f != fuel)
            {
                if (oldP3f < (fuel - changespeed))
                {
                    oldP3f += changespeed;
                }
                else if (oldP3f > (fuel + changespeed))
                {
                    oldP3f -= changespeed;
                }
                if (oldP3f < 0)
                {
                    oldP3f = 0;
                }
            }
            lost = 100.0f - oldP3f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost + (newh / 2))); //height
            ImGui::Dummy(ImVec2(width / 40, 0)); //width
            ImGui::SameLine();
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP3f));
            fuelcount++;
        }
        else if (id_ == 3)
        {
            if (oldP4f != fuel)
            {
                if (oldP4f < (fuel - changespeed))
                {
                    oldP4f += changespeed;
                }
                else if (oldP4f > (fuel + changespeed))
                {
                    oldP4f -= changespeed;
                }
                if (oldP4f < 0)
                {
                    oldP4f = 0;
                }
            }
            lost = 100.0f - oldP4f;
            ImGui::Dummy(ImVec2(0, (float)newunit * lost + (fuelcount % 2) * (newh / 2))); //height
            if (fuelcount >= 2)
            {
                ImGui::Dummy(ImVec2(width - width / 25 - width / 120, 0)); //width
                ImGui::SameLine();
            }
            ImGui::ColorButton("", ImVec4(lost / 100.0f, lost / 100.0f, lost / 100.0f, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(width / 120, (float)newunit * oldP4f));
            fuelcount++;
        }
    }

}

PAGuiConnect::PAGuiConnect()
{
}

PAGuiConnect::~PAGuiConnect() {
}

void PAGuiConnect::update()
{
    int width = Utility::GetWindowWidth();
    int height = Utility::GetWindowHeight();
    int neww = width / 4;
    int newh = height / 4;

    ImGui::Image(GetEngineRender()->GetTextureView(L"../PAEngine/data/textures/connect.png"), ImVec2(neww, newh - 10));

}
