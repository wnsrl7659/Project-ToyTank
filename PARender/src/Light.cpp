#include "PARenderPCH.h"
#include "Light.h"

namespace Rendering {

Light::Light() : mProps(Properties{})
{
}

Light::Properties Light::GetProperties() const
{
  return mProps;
}
void Light::SetProperties(Properties props)
{
  mProps = props;
}

void Light::Enable(bool enable)
{
  mEnabled = enable;
}

Light::__packed_props Light::GetPackedProps()
{
  __packed_props packed{};
  packed.position.m128_f32[0] = mProps.position.x;
  packed.position.m128_f32[1] = mProps.position.y;
  packed.position.m128_f32[2] = mProps.position.z;
  packed.color.m128_f32[0] = mProps.color.r;
  packed.color.m128_f32[1] = mProps.color.g;
  packed.color.m128_f32[2] = mProps.color.b;
  packed.direction.m128_f32[0] = mProps.direction.x;
  packed.direction.m128_f32[1] = mProps.direction.y;
  packed.direction.m128_f32[2] = mProps.direction.z;
  packed.innerAngle = mProps.spotlight_angles.inner;
  packed.outerAngle = mProps.spotlight_angles.outer;
  packed.intensity = mProps.intensity;
  packed.type = static_cast<int>(mProps.type);

  return packed;
}


}
