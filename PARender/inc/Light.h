/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

#include "PARenderAPI/ILight.h"
#include <DirectXMath.h>

namespace Rendering {

class Light :public ILight
{
public:
  Light();
  virtual Properties GetProperties() const override;
  virtual void SetProperties(Properties props) override;
  virtual void Enable(bool enable) override;

  struct __packed_props
  {
    DirectX::XMVECTOR position;  //16
    DirectX::XMVECTOR direction; //16
    DirectX::XMVECTOR color;     //16
    float innerAngle;            //4
    float outerAngle;            //4
    float intensity;             //4
    int type;                    //4
  }; //64 bytes
  
  __packed_props GetPackedProps();

private:
  ILight::Properties mProps;
  bool mEnabled = true;
};
}
