#include "PARenderPCH.h"
#include "Scene.h"
#include "Light.h"
#include "Model.h"

namespace Rendering
{
  Scene::Scene()
  {
    ILight::Properties dir_props{};
    dir_props.color.r = 1.0f;
    dir_props.color.g = 1.0f;
    dir_props.color.b = 1.0f;
    dir_props.type = ILight::Type::dir;
    dir_props.intensity = 0.80f;
    dir_props.direction.y = -1.0f;
    dir_props.direction.z = 5.0f;
    dir_props.direction.x = 2.8f;

    ILight* dir = AddLight();
    dir->SetProperties(dir_props);
  }

  void Scene::AddModel(IModel* model)
  {
#if _DEBUG
      std::string comp = "..\\PARender\\bin\\Debug\\GBufferPixel.cso";
#else
      std::string comp = "..\\PARender\\bin\\Release\\GBufferPixel.cso";
#endif
    if(model->FragmentShader() == comp)
       mModels.push_back(reinterpret_cast<Assets::Model*>(model));
    else
       mModelsForward.push_back(reinterpret_cast<Assets::Model*>(model));
  }

  ILight* Scene::AddLight()
  {
    mLights.emplace_back(new Light);
    return mLights.back();
  }

  void Scene::RemoveLight(ILight* pLight)
  {
    auto it = std::find(mLights.begin(), mLights.end(), pLight);
    
    if (it != mLights.end())
    {
      mLights.erase(it);
    }
  }

  ICamera* Scene::AddCam(float xPos, float yPos, float zPos, float xUp, float yUp,
  float zUp, bool back, float fov, float pitch, float yaw, float aspectRatio, float nearPlane, float farPlane)
  {
      Camera* cam = new Camera({ xPos, yPos, zPos }, { xUp, yUp, zUp }, fov, pitch, yaw, aspectRatio, nearPlane, farPlane);
      if (back && mCams.size() > 0)
      {
          mMiniCams.push_back(cam);
      }
      else
      {
          mCams.push_back(cam);
      }
    
    return reinterpret_cast<ICamera*>(cam);
  }

  void Scene::RemoveCam(ICamera* icam)
  {

      Camera* cam = reinterpret_cast<Camera*>(icam);
      auto it = std::find(mCams.begin(), mCams.end(), cam);
      bool removed = false;

      if (it != mCams.end())
      {
          mCams.erase(it);
          removed = true;
      }
      
      auto it2 = std::find(mMiniCams.begin(), mMiniCams.end(), cam);

      if (it2 != mMiniCams.end())
      {
          mMiniCams.erase(it2);
          removed = true;
      }
  }

  void Scene::Clear()
  {
  }

  std::vector<Camera*>* Scene::GetMiniCameras()
  {
      return &mMiniCams;
  }

  std::vector<Camera*>* Scene::GetActiveCameras()
  {
    return &mCams;
  }

  std::vector<Assets::Model*> Scene::GetModels() const
  {
    return mModels;
  }

  std::vector<Light::__packed_props> Scene::GetPackedLights() const
  {
    std::vector<Light::__packed_props> lightProps;

    for (auto const& light : mLights)
    {
      lightProps.push_back(reinterpret_cast<Light*>(light)->GetPackedProps());
    }

    return lightProps;
  }
}
