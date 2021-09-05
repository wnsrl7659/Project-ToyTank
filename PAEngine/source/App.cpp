/**
 * @file App.cpp
 *
 * @brief An implementation of App.h
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"

#include "App.h"
#include "ServiceLocator.h"
#include "Physics.h"
#include "ObjectManager.h"
#include "SoundSystem.h"
#include "PAGui.h"
#include "CameraManager.h"
#include "EnvironmentManager.h"
#include "PerformanceMonitor.h"

static Rendering::IEngine* engineRender;
static Rendering::IScene* sceneRender;

static ObjectManager* objectManager;
PAEngine::App* PAEngine::App::globalApp = nullptr;

static std::vector<StopWatch> fpsWatcher;
const std::vector<float> GetFPStimes()
{
  std::vector<float> times;
  for (StopWatch watch : fpsWatcher)
  {
    times.push_back(watch.getAverage());
  }

  return times;
}

Rendering::IEngine* GetEngineRender()
{
  return engineRender;
}
Rendering::IScene* GetSceneRender()
{
  return sceneRender;
}


namespace PAEngine
{
  App::App(HINSTANCE _hInstance) : m_appHInstance(_hInstance), m_pAppWindow(NULL),
    hasStarted(false), isDisabled(false), isPaused(false), m_isContinueRunning(true),
    timer(NULL), m_fps(0), m_mspf(0.0), m_gameLogicMaxSkipFrames(10)
  {
    if (globalApp != nullptr)
    {
      return;
    }
    globalApp = this;

    // MUST BE the top of other objects' init
    // create logger
    std::shared_ptr<Util::Logger> engineLogger(new Util::Logger());
    // register the logging service
    Util::ServiceLocator::provideLoggingService(engineLogger);

    // initialize for Timer
    timer = new Util::Timer();
    m_gameLogicTargetFPS = 60.f;
    m_dt = 1.f / m_gameLogicTargetFPS;

    // create an application window
    m_pAppWindow = new Window(this);

    // initialize an event processor
    m_pEventProc = new EventProcessor();

    // MUST BE the below of EventProcessor's init
    // create GameInput
    std::shared_ptr<GameInput> engineGameInput(new GameInput());
    // register the GameInput service
    Util::ServiceLocator::provideGameInputService(engineGameInput);

    // initialize a scene
    m_pSceneManager = new SceneManager(m_pAppWindow, m_pEventProc);
    m_pSceneManager->AddScene("Test", ST_test);
    m_pSceneManager->AddScene("player", ST_playerTest);
    m_pSceneManager->AddScene("castle", ST_castle);
    m_pSceneManager->AddScene("menu", ST_menu);
    m_pSceneManager->AddScene("digipen", ST_digipen);
    m_pSceneManager->SetFirstScene("digipen");

    hasStarted = true;
  }

  App::~App()
  {
    if (m_pSceneManager)
    {
      delete m_pSceneManager;
      m_pSceneManager = nullptr;
    }

    if (m_pEventProc)
    {
      delete m_pEventProc;
      m_pEventProc = nullptr;
    }

    if (m_pAppWindow)
    {
      delete m_pAppWindow;
      m_pAppWindow = nullptr;
    }

    if (timer)
    {
      delete timer;
      timer = NULL;
    }

    if (objectManager) {
      delete objectManager;
    }

    // do not delete
    if (m_appHInstance)
      m_appHInstance = nullptr;
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////// Initialzing //////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////
  bool App::Init()
  {
    // set up timers
    for (int i = 0; i < 7; i++)
    {
      fpsWatcher.push_back(StopWatch());
      fpsWatcher[i].start(60);
    }
    engineRender = Rendering::RenderEngineInstance();
    Rendering::IEngine::PARenderInfo renderInfo{};
    engineRender->Initialize(renderInfo);
    sceneRender = engineRender->CreateScene();
    
    objectManager = ObjectManager::GetInstance();

    // create cameras - need a way to determine how many camera we need
    InitCameraManager(sceneRender);

    InitPhysics();

    EnvironmentManager::getEnvironment()->init();

    SoundSystem* soundSystem = new SoundSystem();
    soundSystem->Init();
    
    mygui = new PAGui;
    mygui->Init(m_pAppWindow->mainWindow, engineRender->Device(), engineRender->Context());
    
    m_pSceneManager->Init();

    // TODO -- move to some kind of camera manager
    
    float width = 1920 / 2.f;
    float height = 1080.0 / 2.f;

    

    return true;
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////// Shutdown ///////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////
  void App::Shutdown()
  {
    m_pSceneManager->Shutdown();
    SoundSystem::Get()->Shutdown();
	  mygui->Shutdown();
    engineRender->Shutdown();
    objectManager->Shutdown();
    ShutdownCameraManager();
    EnvironmentManager::getEnvironment()->shutdown();

    delete mygui;
  }

  int App::Run()
  {
    // reset (start) the timer
    timer->Reset();

    double accumulatedTime = 0.0;		// stores the time accumulated by the rendered
    int nLoops = 0;						// the number of completed loops while updating the game

    // enter main event loop
    MSG msg = { 0 };

    // enter main event loop
    while (m_isContinueRunning)
    {
      m_pSceneManager->CheckScene();
      // peek for messages
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT)
          m_isContinueRunning = false;
      }

      // let the timer tick
      timer->Tick();

      if (!isDisabled)
      {
        // compute fps in the game system loop
        // WARNING! You should remove this function in the game logic to use
        //CalculateFrameStatistics();

        // acquire input
        AcquireInput(m_dt);

        // accumulate the elapsed time since the last frame
        accumulatedTime += timer->GetDeltaTime();

        // now update the game logic with fixed dt as often as possible
        nLoops = 0;
        while (accumulatedTime >= m_dt && nLoops < m_gameLogicMaxSkipFrames)
        {

          // compute fps in the game logic
          CalculateFrameStatistics();

          Update(m_dt);			// game loic loop
          PhysicsUpdate(m_dt);	// game physics loop

          accumulatedTime -= m_dt;
          nLoops++;
        }

         mygui->Update();
     
         // peek into the future and generate the output
         // ref. "fabiensanglard.net/timer_and_framerate/index.php"
         Render(accumulatedTime / m_dt);

         // 03/03/2020 Minsu removed this lines to use the full access on CPU
         //// this accumulatedTime is actually "accumulatedTime - (m_dt*nLoops)"
         //if (nLoops < 2)
         //  Sleep(accumulatedTime * 1000);
      }
    }
    return (int)msg.wParam;
  }

  void App::AcquireInput(const double deltaTime)
  {
    m_pSceneManager->AcquireInput(deltaTime);
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////// Update (Game Logic) /////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////
  void App::Update(const double deltaTime)
  {
    fpsWatcher[0].tick();
    m_pSceneManager->Update(deltaTime);
    fpsWatcher[0].tick();

    fpsWatcher[1].tick();
    objectManager->Update(deltaTime);
    fpsWatcher[1].tick();

    fpsWatcher[2].tick();
    SoundSystem::Get()->Update();
    EnvironmentManager::getEnvironment()->update(deltaTime);
    fpsWatcher[2].tick();
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////// Update (Game Physics) ////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////
  void App::PhysicsUpdate(const double deltaTime)
  {
    if (isPaused)
      return;
    fpsWatcher[3].tick();
    UpdatePhysics(deltaTime);
    fpsWatcher[3].tick();

    fpsWatcher[4].tick();
    UpdateCameraManager(deltaTime);
    fpsWatcher[4].tick();
  }

    /////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// Rendering (Graphics) /////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////

    void App::Render(double farseer)
    {
      fpsWatcher[5].tick();
      engineRender->StartFrame(farseer);
      objectManager->Draw();
      engineRender->RenderScene(farseer);
      mygui->Render();
      engineRender->EndFrame(farseer);
      fpsWatcher[5].tick();
    }

  /////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////// Resizing (Graphics) //////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////
  void App::OnResize()
  {

  }

  /////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////// Utility Functions ////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////

  void App::CalculateFrameStatistics()
  {
    static int nFrames;			// number of frames seen
    static double elapsedTime;	// time since last call
    nFrames++;

    // compute AVERAGE statistics over one second
    // if it computes in every frame, it will cause severe frame dropping
    if ((timer->GetTotalTime() - elapsedTime) >= 1.0)
    {
      // set fps and mspf
      m_fps = nFrames;
      m_mspf = 1000.0 / (double)m_fps;

      // show statistics on window caption (a temporary method)
      std::wstring windowCaption = L"PAEngine --- fps: " + std::to_wstring(m_fps) + L" --- mspf: " + std::to_wstring(m_mspf);
      SetWindowText(m_pAppWindow->GetMainWindowHandle(), windowCaption.c_str());

      // reset
      nFrames = 0;
      elapsedTime += 1.0;
    }
  }

  void App::ChangeScene(std::string name)
  {
    m_pSceneManager->ChangeScene(name);
    EnvironmentManager::getEnvironment()->restart();
  }
  void App::RestartScene()
  {
    m_pSceneManager->RestartScene();
    EnvironmentManager::getEnvironment()->restart();
  }
}