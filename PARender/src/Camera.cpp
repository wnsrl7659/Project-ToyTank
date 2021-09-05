/***************************************************************************/
/*!
\file   Camera.cpp
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

/*============================================================================*\
|| ------------------------------ INCLUDES ---------------------------------- ||
\*============================================================================*/

#include "PARenderPCH.h"
#include "Camera.h"

/*===========================================================================*\
|| --------------------------- GLOBAL VARIABLES ---------------------------- ||
\*===========================================================================*/

/*===========================================================================*\
|| -------------------------- STATIC FUNCTIONS ----------------------------- ||
\*===========================================================================*/

/*===========================================================================*\
|| -------------------------- PUBLIC FUNCTIONS ----------------------------- ||
\*===========================================================================*/

/****************************************************************************/
/*!
\brief
  Default constructor for the camera class

\param pos
  translation

\param up
  up vector

\param fov
  field of view (radians)

\param pitch
  vertical angle (radians)

\param yaw
  horizontal angle (radians)

*/
/****************************************************************************/
Camera::Camera(DirectX::XMVECTOR pos, DirectX::XMVECTOR up, float fov, float pitch,
    float yaw, float aspectRatio, float nearPlane, float farPlane) : position_(pos),
    up_(up), fov_(fov), pitch_(pitch), yaw_(yaw), aspectRatio_(aspectRatio), near_(nearPlane),
    far_(farPlane), frontDirty_(true), viewDirty_(true), projectionDirty_(true)
{
    UpdateMatricies();
}

/****************************************************************************/
/*!
\brief
  memory cleanup

*/
/****************************************************************************/
Camera::~Camera()
{
}

/****************************************************************************/
/*!
\brief
  recalculate all "dirty" matricies

*/
/****************************************************************************/
void Camera::UpdateDirtyMatricies()
{
    if (frontDirty_)
    {
        CalculateFrontVector();
    }

    if (viewDirty_)
    {
        CalculateViewMatrix();
    }

    if (projectionDirty_)
    {
        CalculateProjectionMatrix();
    }
}

/****************************************************************************/
/*!
\brief
  recalculate all matricies

*/
/****************************************************************************/
void Camera::UpdateMatricies()
{
    CalculateFrontVector();
    CalculateViewMatrix();
    CalculateProjectionMatrix();
}

/****************************************************************************/
/*!
\brief
  recalculate only the view matrix

*/
/****************************************************************************/
void Camera::UpdateViewMatrix()
{
    CalculateViewMatrix();
}

/****************************************************************************/
/*!
\brief
  recalculate only the projection matrix

*/
/****************************************************************************/
void Camera::UpdateProjectionMatrix()
{
    CalculateProjectionMatrix();
}

/****************************************************************************/
/*!
\brief
  recalculate only the front vector

*/
/****************************************************************************/
void Camera::UpdateFrontVector()
{
    CalculateFrontVector();
}

/****************************************************************************/
/*!
\brief
  set the active viewport as this camera's viewport

\param context
  pointer to the device context
*/
/****************************************************************************/
void Camera::UseViewport(ID3D11DeviceContext* context) const
{
    context->RSSetViewports(1, &viewPort_);
}

/****************************************************************************/
/*!
\brief
  Set the camera's position

\param pos
  translation
*/
/****************************************************************************/
void Camera::SetPosition(DirectX::XMVECTOR pos)
{
    viewDirty_ = true;
    position_ = pos;
}

/****************************************************************************/
/*!
\brief
  Set the camera's up vector

\param up
  up vector
*/
/****************************************************************************/
void Camera::SetUpVector(DirectX::XMVECTOR up)
{
    viewDirty_ = true;
    up_ = up;
}

/****************************************************************************/
/*!
\brief
  Set the camera's up vector

\param x
  x Position

\param y
  y Position

\param z
  z Position
*/
/****************************************************************************/
void Camera::SetPosition(float x, float y, float z)
{
    viewDirty_ = true;
    position_ = { x, y, z };
}

/****************************************************************************/
/*!
\brief
  Set the camera's up vector

\param x
  x direction

\param y
  y direction

\param z
  z direction
*/
/****************************************************************************/
void Camera::SetUpVector(float x, float y, float z)
{
    viewDirty_ = true;
    up_ = { x, y, z };
}

/****************************************************************************/
/*!
\brief
  Set the camera's field of view

\param fov
  field of view (radians)
*/
/****************************************************************************/
void Camera::SetFOV(float fov)
{
    projectionDirty_ = true;
    fov_ = fov;
}

/****************************************************************************/
/*!
\brief
  Set the camera's pitch

\param pitch
  vertical angle (radians)
*/
/****************************************************************************/
void Camera::SetPitch(float pitch)
{
    frontDirty_ = true;
    pitch_ = pitch;
}

/****************************************************************************/
/*!
\brief
  Set the camera's yaw

\param yaw
  horizontal angle (radians)
*/
/****************************************************************************/
void Camera::SetYaw(float yaw)
{
    frontDirty_ = true;
    yaw_ = yaw;
}

/****************************************************************************/
/*!
\brief
  Set the camera's aspect ratio from a numerator and denmoniator

\param numerator
  top of the ratio

\param denominator
  bottom of the ratio
*/
/****************************************************************************/
void Camera::SetAspectRatio(float numerator, float denominator)
{
    projectionDirty_ = true;
    aspectRatio_ = numerator / denominator;
    viewWidth_ = numerator;
    viewHeight_ = denominator;
}

/****************************************************************************/
/*!
\brief
  Set the camera's aspect ratio

\param aspectRatio
  the precalculated aspect ratio
*/
/****************************************************************************/
//void Camera::SetAspectRatio(float aspectRatio)
//{
//    projectionDirty_ = true;
//    aspectRatio_ = aspectRatio;
//}

/****************************************************************************/
/*!
\brief
  Set the camera's far plane

\param farPlane
  the z value of the far plane
*/
/****************************************************************************/
void Camera::SetFarPlane(float farPlane)
{
    projectionDirty_ = true;
    far_ = farPlane;
}

/****************************************************************************/
/*!
\brief
  Set the camera's near plane

\param farPlane
  the z value of the near plane
*/
/****************************************************************************/
void Camera::SetNearPlane(float nearPlane)
{
    projectionDirty_ = true;
    near_ = nearPlane;
}

/****************************************************************************/
/*!
\brief
  Set the camera's view port

\param vp
  the viewport to copy data from
*/
/****************************************************************************/
void Camera::SetViewPort(D3D11_VIEWPORT& vp)
{
    viewPort_.Width = vp.Width;
    viewPort_.Height = vp.Height;
    viewPort_.TopLeftX = vp.TopLeftX;
    viewPort_.TopLeftY = vp.TopLeftY;
    viewPort_.MinDepth = vp.MinDepth;
    viewPort_.MaxDepth = vp.MaxDepth;
}

/****************************************************************************/
/*!
\brief
  Set the camera's view port

\param width
   viewport width

\param height
   viewport height

\param topLeftX
   X position of the viewport

\param topLeftY
   Y position of the viewport

\param minDepth
   minimum depth of the viewport

\param maxDepth
   maximum depth of the viewport
*/
/****************************************************************************/
void Camera::SetViewPort(float width, float height, float topLeftX, float topLeftY, float minDepth, float maxDepth)
{
    viewPort_.Width = FLOAT(width);
    viewPort_.Height = FLOAT(height);
    viewPort_.TopLeftX = FLOAT(topLeftX);
    viewPort_.TopLeftY = FLOAT(topLeftY);
    viewPort_.MinDepth = FLOAT(minDepth);
    viewPort_.MaxDepth = FLOAT(maxDepth);
}

/****************************************************************************/
/*!
\brief
  Set the camera's front vector

\param front
  the new front vector
*/
/****************************************************************************/
void Camera::SetFront(DirectX::XMVECTOR front)
{
    front_ = DirectX::XMVector3Normalize(front);
    frontDirty_ = false;
}

void Camera::UseOrthoMatrix(bool b)
{
    useOrtho_ = b;
}

/****************************************************************************/
/*!
\brief
  Get the camera's view matrix

\return
  the camera's view matrix
*/
/****************************************************************************/
DirectX::XMMATRIX Camera::ViewMatrix() const
{
    return viewMatrix_;
}

/****************************************************************************/
/*!
\brief
  Get the camera's projection matrix (has unique fov)

\return
  the camera's projection matrix
*/
/****************************************************************************/
DirectX::XMMATRIX Camera::ProjectionMatrix() const
{
    return projectionMatrix_;
}

DirectX::XMMATRIX Camera::OrthoMatrix() const
{
    return orthoMatrix_;
}

/****************************************************************************/
/*!
\brief
  Get the camera's position

\return
  the camera's position
*/
/****************************************************************************/
std::vector<float> Camera::Position() const
{
    DirectX::XMFLOAT3 f;
    DirectX::XMStoreFloat3(&f, position_);
    return { f.x, f.y, f.z };
}

/****************************************************************************/
/*!
\brief
  Get the camera's up vector

\return
  the camera's up vector
*/
/****************************************************************************/
std::vector<float> Camera::Up() const
{
    DirectX::XMFLOAT3 f;
    DirectX::XMStoreFloat3(&f, up_);
    return { f.x, f.y, f.z };
}

/****************************************************************************/
/*!
\brief
  Get the camera's front vector (based on pitch and yaw)

\return
  the camera's front vector
*/
/****************************************************************************/
std::vector<float> Camera::Front() const
{
    DirectX::XMFLOAT3 f;
    DirectX::XMStoreFloat3(&f, front_);
    return { f.x, f.y, f.z };
}

/****************************************************************************/
/*!
\brief
  Get the camera's field of view

\return
  the camera's field of view
*/
/****************************************************************************/
float Camera::FOV() const
{
    return fov_;
}

/****************************************************************************/
/*!
\brief
  Get the camera's pitch (vertical angle)

\return
  the camera's pitch (radians)
*/
/****************************************************************************/
float Camera::Pitch() const
{
    return pitch_;
}

/****************************************************************************/
/*!
\brief
  Get the camera's yaw (horizontal angle)

\return
  the camera's yaw (radians)
*/
/****************************************************************************/
float Camera::Yaw() const
{
    return yaw_;
}

/****************************************************************************/
/*!
\brief
  Get the camera's aspect ratio

\return
  the camera's aspect ratio
*/
/****************************************************************************/
float Camera::AspectRatio() const
{
    return aspectRatio_;
}

/****************************************************************************/
/*!
\brief
  Get the camera's near plane

\return
  the camera's near plane
*/
/****************************************************************************/
float Camera::NearPlane() const
{
    return near_;
}

/****************************************************************************/
/*!
\brief
  Get the camera's far plane

\return
  the camera's far plane
*/
/****************************************************************************/
float Camera::FarPlane() const
{
    return far_;
}

/****************************************************************************/
/*!
\brief
  Get the camera's View Port

\return
  the camera's View Port
*/
/****************************************************************************/
D3D11_VIEWPORT Camera::ViewPort() const
{
    return viewPort_;
}

/*===========================================================================*\
|| -------------------------- PRIVATE FUNCTIONS ---------------------------- ||
\*===========================================================================*/

/****************************************************************************/
/*!
\brief
  calculate a new view matrix
*/
/****************************************************************************/
void Camera::CalculateViewMatrix()
{
    viewDirty_ = false;
    viewMatrix_ = DirectX::XMMatrixLookAtLH(position_,
        DirectX::XMVectorAdd(position_, front_), up_);
}

/****************************************************************************/
/*!
\brief
  calculate a new projection matrix
*/
/****************************************************************************/
void Camera::CalculateProjectionMatrix()
{
    projectionDirty_ = false;

    if (useOrtho_)
    {
        orthoMatrix_ = DirectX::XMMatrixOrthographicLH(viewWidth_, viewHeight_, near_, far_);
    }
    else
    {
        projectionMatrix_ = DirectX::XMMatrixPerspectiveFovLH(fov_, aspectRatio_, near_, far_);
    }
}

/****************************************************************************/
/*!
\brief
  calculate a new front vector
*/
/****************************************************************************/
void Camera::CalculateFrontVector()
{
    viewDirty_ = true;
    frontDirty_ = false;
    front_ = DirectX::XMVector3Normalize({ std::sin(yaw_) * std::cos(pitch_),
     std::sin(pitch_),   -std::cos(yaw_) * std::cos(pitch_) });
}
