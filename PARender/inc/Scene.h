/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

#include "PARenderAPI/IScene.h"
#include "Camera.h"
#include "Light.h"

namespace Rendering { namespace Assets {
class Model;
} //namespace assets
} //namespace rendering

namespace Rendering {
  class Scene : public IScene {
  public:
  Scene();

  virtual void AddModel(IModel* model) override;
  virtual ILight* AddLight() override;
  virtual void RemoveLight(ILight*) override;
  virtual ICamera* AddCam(float xPos = 0, float yPos = 0, float zPos = 4, float xUp = 0, float yUp = 1, float zUp = 0, bool back = false, float fov = 1.0472f,
    float pitch = 0, float yaw = 0, float aspectRatio = 16 / 9, float nearPlane = 0.1f, float farPlane = 1000.f) override;
  virtual void Clear() override;
  virtual void RemoveCam(ICamera* icam) override;

  std::vector<Camera*>* GetMiniCameras();
  std::vector<Camera*>* GetActiveCameras();
  std::vector<Assets::Model*> GetModels() const;
  std::vector<Assets::Model*> GetForwardModels() const { return mModelsForward; };
  std::vector<Light::__packed_props> GetPackedLights() const;
  
  private:
  std::vector<Camera*> mMiniCams;
  std::vector<Camera*> mCams;
  std::vector<Assets::Model*> mModels;
  std::vector<Assets::Model*> mModelsForward;
  std::vector<ILight*> mLights;
  };
}
