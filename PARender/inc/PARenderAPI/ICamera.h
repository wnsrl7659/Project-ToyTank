/***************************************************************************/
/*!
\file   Icamera.h
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

#ifndef _ICAMERA_H
#define _ICAMERA_H
#pragma once

#include <vector>

class ICamera
{

public:

    /* core */
    virtual ~ICamera() = default;

    /* setters */
    virtual void SetPosition(float x, float y, float z) = 0;
    virtual void SetUpVector(float x, float y, float z) = 0;
    virtual void SetFOV(float fov) = 0;
    virtual void SetPitch(float pitch) = 0;
    virtual void SetYaw(float yaw) = 0;
    virtual void SetAspectRatio(float numerator, float denominator) = 0;
    //virtual void SetAspectRatio(float aspectRatio) = 0;
    virtual void SetFarPlane(float farPlane) = 0;
    virtual void SetNearPlane(float nearPlane) = 0;
    virtual void SetViewPort(float width, float height, float topLeftX, float topLeftY, float minDepth, float maxDepth) = 0;

    virtual void UseOrthoMatrix(bool b) = 0;

    /* getters */
    virtual std::vector<float> Position() const = 0;
    virtual std::vector<float> Up() const = 0;
    virtual std::vector<float> Front() const = 0;
    virtual float FOV() const = 0;
    virtual float Pitch() const = 0;
    virtual float Yaw() const = 0;
    virtual float AspectRatio() const = 0;
    virtual float NearPlane() const = 0;
    virtual float FarPlane() const = 0;

private:
};

#endif