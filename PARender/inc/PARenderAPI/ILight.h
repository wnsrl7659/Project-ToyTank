/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

namespace Rendering {

class ILight {
public:
  enum class Type : int
  {
    point, spot, dir
  };

  struct Properties
  {
    union Position
    {
      struct {
        float x, y, z;
      };
      float _v[3];
    }position;

    union Color
    {
      struct {
        float r, g, b;
      };
      float _c[3];
    }color;

    union Direction
    {
      struct {
        float x, y, z;
      };
      float _d[3];
    }direction;

    union SpotlightAngles
    {
      struct {
        float inner, outer;
      };
      float _sla[2];
    }spotlight_angles;

    float intensity;
    Type type = Type::point;
  };

  virtual ~ILight() = default;
  virtual Properties GetProperties() const = 0;
  virtual void SetProperties(Properties props) = 0;
  virtual void Enable(bool enable) = 0;
private:
};
}
