/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

#include "RenderTypes.h"
#include "ICamera.h"

namespace Rendering {
  class IModel;
  class ILight;
} // namespace Rendering


namespace Rendering {

  class IScene
  {
  public:
  virtual ~IScene() = default;
  virtual void AddModel(IModel* model) = 0;
  virtual ILight* AddLight() = 0;
  virtual void RemoveLight(ILight*) = 0;
  virtual void RemoveCam(ICamera* icam) = 0;

  virtual ICamera* AddCam(float xPos = 0, float yPos = 0, float zPos = 4, float xUp = 0, float yUp = 1, float zUp = 0, bool back = false, float fov = 1.0472f,
    float pitch = 0, float yaw = 0, float aspectRatio = 16 / 9, float nearPlane = 0.1f, float farPlane = 1000.f) = 0;
  virtual void Clear() = 0;

  //  virtual ICamera* GetActiveCameras(int& count) const = 0;
  private:
  };

} // namespace Rendering
