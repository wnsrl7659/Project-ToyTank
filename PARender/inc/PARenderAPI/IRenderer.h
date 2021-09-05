/***************************************************************************/
/*!
\author Ryan Dugie, Seth Kohler

(c) DigiPen Institute of Technology 2019
*/
/***************************************************************************/

#pragma once

namespace Rendering {
  class IScene;
} // namespace Rendering

namespace Rendering {
  
  class IRenderer
  {
  public:
  virtual ~IRenderer() = default;
  virtual void Render(IScene*) = 0;
  private:
  };
} // namespace Rendering
