/******************************************************************************
  file: ChildTransformManager.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"

#include "ChildTransformManager.h"
#include "ChildTransform.h"
#include "Transform.h"
#include "ObjectManager.h"
#include <DirectXMath.h>

TransformManager* TransformManager::instance_ = nullptr;

TransformManager::TransformManager()
  : ComponentManager() {

}

TransformManager* TransformManager::GetInstance() {
  if (!instance_) {
    instance_ = new TransformManager();
  }

  return instance_;
}

TransformManager::~TransformManager() {

}

void TransformManager::Init() {
  //  allocate memory for ChildTransforms on base classes array
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    components_[i] = new Transform(i, this);
  }
}

static void MultMat(float* result, float* a, float* b) {
  DirectX::XMMATRIX matrix1(a);
  DirectX::XMMATRIX matrix2(b);
  DirectX::XMMATRIX multResult = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranspose(matrix1), matrix2);
  for (unsigned i = 0; i < 16; ++i) {
    result[i] = multResult.r->m128_f32[i];
  }
}

void TransformManager::Update(float dt) {
  //  update the transform matrix based on the relative matrix and the parents Transform
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    if (allocated_[i])     {
      Transform* transform = dynamic_cast<Transform*>(components_[i]);
      GameObject* thisObject = ObjectManager::GetObjectByID(transform->GetParent());
      Physics* physComponent = thisObject->GetComponent<Physics>();

      //  if there is no box collider on this object, find the box collider on the parent object
      //  copy the matrix from the box collider to the matrix stored in transform
      if (!physComponent) {
      int parentID = thisObject->GetParent();
      physComponent = ObjectManager::GetObjectByID(parentID)->GetComponent<Physics>();
      }

      MultMat(transform->transformMatrix_.data(),
              transform->relativeMatrix_.data(),
              physComponent->GetMatrix().data());

    }
  }
}

void TransformManager::Draw() {
}

void TransformManager::Shutdown() {
  //  delete memory allocated in init
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    delete components_[i];
    components_[i] = nullptr;
    allocated_[i] = false;
  }
}
