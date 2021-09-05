/***************************************************************************/
/*!
\file   camera.h
\author Ryan Dugie
\brief  Copyright (c) DigiPen 2019
*/
/***************************************************************************/

#ifndef _CAMERA_H
#define _CAMERA_H
#pragma once

#include <DirectXMath.h>
#include "PARenderAPI/ICamera.h"

class Camera : public ICamera
{

public:

    /* core */
    Camera(DirectX::XMVECTOR pos = { 0, 0, 4 }, DirectX::XMVECTOR up = { 0, 1, 0 }, float fov = 1.22173,
        float pitch = 0, float yaw = 0, float aspectRatio = 16 / 9, float nearPlane = 0.1f, float farPlane = 250.f);
    virtual ~Camera() override;
    void UpdateDirtyMatricies();
    void UpdateMatricies();
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    void UpdateFrontVector();
    void UseViewport(ID3D11DeviceContext* context) const;

    /* setters */
    void SetPosition(DirectX::XMVECTOR pos);
    void SetUpVector(DirectX::XMVECTOR up);
    virtual void SetPosition(float x, float y, float z) override;
    virtual void SetUpVector(float x, float y, float z) override;
    virtual void SetFOV(float fov) override;
    virtual void SetPitch(float pitch) override;
    virtual void SetYaw(float yaw) override;
    virtual void SetAspectRatio(float numerator, float denominator) override;
    // virtual void SetAspectRatio(float aspectRatio) override;
    virtual void SetFarPlane(float farPlane) override;
    virtual void SetNearPlane(float nearPlane) override;
    void SetViewPort(D3D11_VIEWPORT& vp);
    virtual void SetViewPort(float width, float height, float topLeftX, float topLeftY, float minDepth, float maxDepth) override;
    void SetFront(DirectX::XMVECTOR front);

    virtual void UseOrthoMatrix(bool b) override;

    /* getters */
    DirectX::XMMATRIX ViewMatrix() const;
    DirectX::XMMATRIX ProjectionMatrix() const;
    DirectX::XMMATRIX OrthoMatrix() const;
    virtual std::vector<float> Position() const override;
    virtual std::vector<float> Up() const override;
    virtual std::vector<float> Front() const override;
    virtual float FOV() const override;
    virtual float Pitch() const override;
    virtual float Yaw() const override;
    virtual float AspectRatio() const override;
    virtual float NearPlane() const override;
    virtual float FarPlane() const override;
    D3D11_VIEWPORT ViewPort() const;
    bool UsingOrtho() const { return useOrtho_; }

private:

    /* functions */
    void CalculateViewMatrix();
    void CalculateProjectionMatrix();
    void CalculateFrontVector();

    /* matricies */
    DirectX::XMMATRIX viewMatrix_;
    DirectX::XMMATRIX projectionMatrix_;
    DirectX::XMMATRIX orthoMatrix_;

    /* vectors */
    DirectX::XMVECTOR position_;
    DirectX::XMVECTOR up_;
    DirectX::XMVECTOR front_;

    /* floats */
    float fov_;
    float pitch_;
    float yaw_;
    float aspectRatio_;
    float near_;
    float far_;
    float viewWidth_ = 100;
    float viewHeight_ = 100;

    /* bools */
    bool frontDirty_;
    bool viewDirty_;
    bool projectionDirty_;
    bool useOrtho_ = false;

    /* misc */
    D3D11_VIEWPORT viewPort_;
};

#endif