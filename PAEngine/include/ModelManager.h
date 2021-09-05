/******************************************************************************
  filename: ModelManager.h
  author: Joseph McDonell

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once
#include <array>
#include "ComponentManager.h"
#include "ModelComponent.h" //  ModelType::MT_NUM

class ModelManager : public ComponentManager {
public:
  static ModelManager* GetInstance();
  ~ModelManager();

  void Init() override;
  void Update(float dt) override;
  void Draw() override;
  void Shutdown() override;

  static Rendering::IModel* GetModel(ModelType modelType);
private:
  ModelManager();
  static ModelManager* instance_;
  static std::array<Rendering::IModel*, MT_NUM> models_;
  static const wchar_t* texturePath;

    void GenerateModelCube(ModelType modelType);
    void GenerateModelSphere(ModelType modelType);
    void GenerateModelQuad(ModelType modelType);

  void SetTexture(ModelType modelType, const wchar_t* albedo = nullptr, const wchar_t* normal = nullptr,
    const wchar_t* rough = nullptr, const wchar_t* metalic = nullptr, const wchar_t* ambiantocclusion = nullptr,
    const wchar_t* heightMap = nullptr);

  friend class ModelComponent;
};