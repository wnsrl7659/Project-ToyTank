/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

#include "RenderTypes.h"

namespace Rendering {
  class IShaderProgram {
  public:
  virtual ProgramID ID() const = 0;
  virtual std::string FragmentShader() const = 0;
  private:
  };
}
