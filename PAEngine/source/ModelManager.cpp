/******************************************************************************
  file: ModelManager.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "ModelManager.h"
#include "ObjectManager.h"
#include "App.h"

#include "Transform.h" 
#include "ChildTransform.h"
#include <vector>

ModelManager* ModelManager::instance_ = nullptr;
std::array<Rendering::IModel*, MT_NUM> ModelManager::models_ = std::array<Rendering::IModel*, MT_NUM>();
const wchar_t* ModelManager::texturePath = L"../PAEngine/data/textures/";

ModelManager::ModelManager() {
    //  generate models
      //  generate models
    GenerateModelCube(MT_MONO_COLOR_CUBE);
    GenerateModelSphere(MT_MONO_COLOR_SPHERE);

    GenerateModelCube(MT_DIGIPEN_LOGO);
    SetTexture(MT_DIGIPEN_LOGO, L"DigiPenLogo.png", L"defaultNormal.png", L"/Tank/streakedmetal-roughness.png",
        L"defaultBlack.png", L"defaultWhite.png", L"defaultBlack.png");

    GenerateModelCube(MT_CUBE_METALGRID);
    SetTexture(MT_CUBE_METALGRID, L"groundMonoColor.png", L"defaultNormal.png", L"/Tank/streakedmetal-roughness.png",
        L"/Tank/streakedmetal-roughness.png", L"/Tank/streakedmetal-albedo.png", L"defaultBlack.png");

    GenerateModelCube(MT_FUEL_DEPOT);
    SetTexture(MT_FUEL_DEPOT, L"/PaintRefill/PaintRefill_Albedo.png", L"defaultNormal.png", L"/wall/rough-plaster-roughness.png",
        L"/wall/rough-plaster-metallic.png", L"/wall/rough-plaster-ao.png", L"/wall/rough-plaster-ao.png");

    GenerateModelSphere(MT_SPHERE);
    SetTexture(MT_SPHERE, L"groundMonoColor.png", L"defaultNormal.png", L"/Tank/streakedmetal-roughness.png",
        L"/Tank/streakedmetal-roughness.png", L"/Tank/streakedmetal-albedo.png", L"defaultBlack.png");

    GenerateModelCube(MT_CUBE_DARK);
    SetTexture(MT_CUBE_DARK, L"grayMedium.png", L"/wall/rough-plaster-normal-dx.png", L"/wall/rough-plaster-roughness.png",
        L"/wall/rough-plaster-metallic.png", L"/wall/rough-plaster-ao.png", L"/wall/rough-plaster-ao.png");

    GenerateModelCube(MT_WALL_LARGE);
    SetTexture(MT_WALL_LARGE, L"/wall/rough-plaster-basecolor.png", L"/wall/rough-plaster-normal-dx.png", L"/wall/rough-plaster-roughness.png",
        L"/wall/rough-plaster-metallic.png", L"/wall/rough-plaster-ao.png", L"/wall/rough-plaster-ao.png");

    GenerateModelCube(MT_WALL_BRICK);
    SetTexture(MT_WALL_BRICK, L"/wall/rough-plaster-basecolor.png", L"/wall/rough-plaster-normal-dx.png", L"/wall/rough-plaster-roughness.png",
        L"/wall/rough-plaster-metallic.png", L"/wall/rough-plaster-ao.png", L"/wall/rough-plaster-ao.png");

    GenerateModelCube(MT_FLOOR_CONCRETE);
    SetTexture(MT_FLOOR_CONCRETE, L"grayMedium.png", L"defaultNormal.png", L"/wall/rough-plaster-roughness.png",
        L"/wall/rough-plaster-metallic.png", L"/wall/rough-plaster-ao.png", L"/wall/rough-plaster-ao.png");
    /*    SetTexture(MT_FLOOR_CONCRETE, L"gravelAlbedo.png", L"gravelNormal.png", L"graniteRough.png",
        L"/wall/rough-plaster-metallic.png", L"gravelAO.png", L"/gravelHeight.png");*/

    GenerateModelCube(MT_FLAG);
    SetTexture(MT_FLAG, L"lightRed.png", L"/wall/rough-plaster-normal-dx.png", L"/wall/rough-plaster-roughness.png",
        L"/wall/rough-plaster-metallic.png", L"/wall/rough-plaster-ao.png", L"/wall/rough-plaster-ao.png");

    GenerateModelCube(MT_GAUGE);
    SetTexture(MT_GAUGE, L"lightBlue.png", L"/wall/rough-plaster-normal-dx.png", L"/wall/rough-plaster-roughness.png",
        L"/wall/rough-plaster-metallic.png", L"/wall/rough-plaster-ao.png", L"/wall/rough-plaster-ao.png");

    GenerateModelCube(MT_TUTORIAL);
    SetTexture(MT_TUTORIAL, L"unknown.png", L"defaultNormal.png", L"defaultBlack.png",
        L"defaultBlack.png", L"defaultWhite.png", L"defaultWhite.png");
}

ModelManager* ModelManager::GetInstance()
{
    if (!instance_) {
        instance_ = new ModelManager();
    }
    return instance_;
}

ModelManager::~ModelManager() {
}

void ModelManager::Init() {
    //  allocate memory for transforms on base classes array
    for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
        components_[i] = new ModelComponent(i, this);
    }
}

void ModelManager::Update(float dt) {
    //do nothing for models
}

void ModelManager::Draw() {
    //  set the transform for each model instance
    for (Component*& c : components_) {
        if (c->GetParent() != -1) {
            ModelComponent* m = reinterpret_cast<ModelComponent*>(c);
            Component* t = ObjectManager::GetObjectByID(m->GetParent())->GetComponent<Transform>();

            if (t) {
                models_[m->modelType_]->SetTransform(m->instance_, reinterpret_cast<Transform*>(t)->GetTransform());
            }
        }
    }
}

void ModelManager::Shutdown() {
    //  delete memory allocated in init
    for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
        delete components_[i];
        components_[i] = nullptr;
        allocated_[i] = false;
    }
}

Rendering::IModel* ModelManager::GetModel(ModelType modelType)
{
    return models_[modelType];
}

void ModelManager::GenerateModelCube(ModelType modelType)
{
    ////////////////////////////////////////////////////////
    float g_Vertices[] =
    {
        -1.0,  1.0, -1.0,
         1.0,  1.0, -1.0,
        -1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,
         1.0,  1.0, -1.0,
         1.0, -1.0, -1.0,

         1.0,  1.0, -1.0,
         1.0,  1.0,  1.0,
         1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0,  1.0,  1.0,
         1.0, -1.0,  1.0,

         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
         1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
        -1.0,  1.0,  1.0,
        -1.0, -1.0,  1.0,

        -1.0,  1.0,  1.0,
        -1.0,  1.0, -1.0,
        -1.0, -1.0,  1.0,
        -1.0, -1.0,  1.0,
        -1.0,  1.0, -1.0,
        -1.0, -1.0, -1.0,

        -1.0,  1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0,
         1.0,  1.0,  1.0,
         1.0,  1.0, -1.0,

        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
        -1.0, -1.0,  1.0,
        -1.0, -1.0,  1.0,
         1.0, -1.0, -1.0,
         1.0, -1.0,  1.0,
    };

    unsigned g_Indicies[36] =
    {
       35,34,33,32,31,30,
       29,28,27,26,25,24,
       23,22,21,20,19,18,
       17,16,15,14,13,12,
       11,10,9,8,7,6,5,4,
       3,2,1,0
    };

    float g_UV[] =
    {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,
    };

    models_[modelType] = GetEngineRender()->GenerateModel("Cube", g_Vertices, 108, g_Indicies, 36, g_UV);

    if (modelType == MT_MONO_COLOR_CUBE)
    {
#if _DEBUG
        Rendering::IShaderProgram* program = GetEngineRender()->CreateShaderProgram("..\\PARender\\bin\\Debug\\GBufferVertex.cso",
            "..\\PARender\\bin\\Debug\\ColorPixel.cso");
#else
        Rendering::IShaderProgram* program = GetEngineRender()->CreateShaderProgram("..\\PARender\\bin\\Release\\GBufferVertex.cso",
            "..\\PARender\\bin\\Release\\ColorPixel.cso");
#endif
        models_[modelType]->SetShaderProgram(program);
    }
    else
    {
#if _DEBUG
        Rendering::IShaderProgram* program = GetEngineRender()->CreateShaderProgram("..\\PARender\\bin\\Debug\\GBufferVertex.cso",
            "..\\PARender\\bin\\Debug\\GBufferPixel.cso");
#else
        Rendering::IShaderProgram* program = GetEngineRender()->CreateShaderProgram("..\\PARender\\bin\\Release\\GBufferVertex.cso",
            "..\\PARender\\bin\\Release\\GBufferPixel.cso");
#endif
        models_[modelType]->SetShaderProgram(program);
    }


    switch (modelType)
    {
    case MT_CUBE_METALGRID:
        models_[modelType]->SetTexCoordScale(1.0f);
        models_[modelType]->SetSeeThrough(false);

        break;

    case MT_CUBE_DARK:
        models_[modelType]->SetTexCoordScale(1.0f);
        models_[modelType]->SetSeeThrough(false);

        break;

    case MT_WALL_LARGE:
        models_[modelType]->SetTexCoordScale(0.2f);
        break;

    case MT_WALL_BRICK:
        models_[modelType]->SetTexCoordScale(1.0f);
        break;

    case MT_FLOOR_CONCRETE:
        models_[modelType]->SetTexCoordScale(0.2f);
        models_[modelType]->SetSeeThrough(false);
        break;

    case MT_GAUGE:
        models_[modelType]->SetTexCoordScale(1.0f);
        models_[modelType]->SetSeeThrough(false);
        break;

    default:
        break;
    }
    models_[modelType]->SetSamplerType(Rendering::IModel::_WRAP_);
    //_WRAP_, _MIRROR_, _CLAMP_

    GetSceneRender()->AddModel(models_[modelType]);
}

void ModelManager::GenerateModelSphere(ModelType modelType)
{
    ////////////////////////////////////////////////////////
    std::vector<float>g_Vertices;
    g_Vertices.reserve(363);
    std::vector<unsigned> g_Indicies;
    g_Indicies.reserve(540);
    std::vector<float> g_UV;
    g_UV.reserve(242);

    vec3 position;
    float pi = 3.14159265359f;
    int divisions = 10;
    float radius = 1.0f;
    float sectorStep = 2.0f * pi / divisions;
    float stackStep = pi / divisions;
    float theta = 0;
    float phi = 0;
    float length = 1.0f / radius;

    // generate vertices
    for (int i = 0; i <= divisions; ++i)
    {
        theta = pi / 2 - i * stackStep;
        float xy = radius * cosf(theta);
        position.z = radius * sinf(theta);

        for (int j = 0; j <= divisions; ++j)
        {
            phi = j * sectorStep;
            position.x = xy * cosf(phi);
            position.y = xy * sinf(phi);

            g_Vertices.push_back(position.x);
            g_Vertices.push_back(position.y);
            g_Vertices.push_back(position.z);

            // gen uvs
            float s = (float)j / divisions;
            float t = (float)i / divisions;
            g_UV.push_back(s);
            g_UV.push_back(t);
        }
    }

    // generate indices
    unsigned k1;
    unsigned k2;
    for (int i = 0; i < divisions; ++i)
    {
        k1 = i * (divisions + 1);   // beginning of current stack
        k2 = k1 + divisions + 1;    // beginning of next stack

        for (int j = 0; j < divisions; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                g_Indicies.push_back(k1);
                g_Indicies.push_back(k2);
                g_Indicies.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (divisions - 1))
            {
                g_Indicies.push_back(k1 + 1);
                g_Indicies.push_back(k2);
                g_Indicies.push_back(k2 + 1);
            }
        }
    }

    models_[modelType] = GetEngineRender()->GenerateModel("Sphere", &g_Vertices.front(), g_Vertices.size(), &g_Indicies.front(), g_Indicies.size(), &g_UV.front());

    if (modelType == MT_MONO_COLOR_SPHERE)
    {
#if _DEBUG
        Rendering::IShaderProgram* program = GetEngineRender()->CreateShaderProgram("..\\PARender\\bin\\Debug\\GBufferVertex.cso",
            "..\\PARender\\bin\\Debug\\ColorPixel.cso");
#else
        Rendering::IShaderProgram* program = GetEngineRender()->CreateShaderProgram("..\\PARender\\bin\\Release\\GBufferVertex.cso",
            "..\\PARender\\bin\\Release\\ColorPixel.cso");
#endif
        models_[modelType]->SetShaderProgram(program);
    }
    else
    {
#if _DEBUG
        Rendering::IShaderProgram* program = GetEngineRender()->CreateShaderProgram("..\\PARender\\bin\\Debug\\GBufferVertex.cso",
            "..\\PARender\\bin\\Debug\\GBufferPixel.cso");
#else
        Rendering::IShaderProgram* program = GetEngineRender()->CreateShaderProgram("..\\PARender\\bin\\Release\\GBufferVertex.cso",
            "..\\PARender\\bin\\Release\\GBufferPixel.cso");
#endif
        models_[modelType]->SetShaderProgram(program);
    }


    switch (modelType)
    {
    case MT_CUBE_METALGRID:
        models_[modelType]->SetTexCoordScale(1.0f);
        models_[modelType]->SetSeeThrough(false);

        break;

    case MT_CUBE_DARK:
        models_[modelType]->SetTexCoordScale(1.0f);
        models_[modelType]->SetSeeThrough(false);

        break;

    case MT_WALL_LARGE:
        models_[modelType]->SetTexCoordScale(0.2f);
        break;

    case MT_WALL_BRICK:
        models_[modelType]->SetTexCoordScale(1.0f);
        break;

    case MT_FLOOR_CONCRETE:
        models_[modelType]->SetTexCoordScale(0.2f);
        models_[modelType]->SetSeeThrough(false);
        break;

    case MT_GAUGE:
        models_[modelType]->SetTexCoordScale(1.0f);
        models_[modelType]->SetSeeThrough(false);
        break;

    default:
        break;
    }
    models_[modelType]->SetSamplerType(Rendering::IModel::_WRAP_);
    //_WRAP_, _MIRROR_, _CLAMP_

    GetSceneRender()->AddModel(models_[modelType]);
}

void ModelManager::GenerateModelQuad(ModelType modelType)
{
    float quadVertices[] = {
    -1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f
    };

    float quadTexCoords[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };

    unsigned quadIndicies[] = {
        0, 1, 3, 3, 2, 0
    };

    models_[modelType] = GetEngineRender()->GenerateModel("Quad", quadVertices, 108, quadIndicies, 36, quadTexCoords);

#if _DEBUG
    Rendering::IShaderProgram* program = GetEngineRender()->CreateShaderProgram("..\\PARender\\bin\\Debug\\GBufferVertex.cso",
        "..\\PARender\\bin\\Debug\\GBufferPixel.cso");
#else
    Rendering::IShaderProgram* program = GetEngineRender()->CreateShaderProgram("..\\PARender\\bin\\Release\\GBufferVertex.cso",
        "..\\PARender\\bin\\Release\\GBufferPixel.cso");
#endif
    models_[modelType]->SetShaderProgram(program);
    models_[modelType]->SetSamplerType(Rendering::IModel::_WRAP_);

    GetSceneRender()->AddModel(models_[modelType]);
}

/*  concatonates a filename to the end of a path, allocates memory for the result  */
static wchar_t* PathConcat(const wchar_t* path, const wchar_t* filename) {
    wchar_t* result = nullptr;
    size_t sizePath = wcslen(path);
    size_t sizefilname = wcslen(filename);

    result = new wchar_t[sizePath + sizefilname + 1];
    *result = L'\0';
    wcscat_s(result, sizePath + sizefilname + 1, path);
    wcscat_s(result, sizePath + sizefilname + 1, filename);

    return result;
}

void ModelManager::SetTexture(ModelType modelType, const wchar_t* albedo, const wchar_t* normal, const wchar_t* rough, const wchar_t* metalic, const wchar_t* ambiantocclusion,
    const wchar_t* heightMap) {
    wchar_t* filePath;

    if (albedo) {
        filePath = PathConcat(texturePath, albedo);
        models_[modelType]->SetTexture(Rendering::IModel::_ALBEDO_, filePath, true);
        delete filePath;
    }
    if (normal) {
        filePath = PathConcat(texturePath, normal);
        models_[modelType]->SetTexture(Rendering::IModel::_NORMAL_, filePath);
        delete filePath;
    }
    if (rough) {
        filePath = PathConcat(texturePath, rough);
        models_[modelType]->SetTexture(Rendering::IModel::_ROUGH_, filePath);
        delete filePath;
    }
    if (metalic) {
        filePath = PathConcat(texturePath, metalic);
        models_[modelType]->SetTexture(Rendering::IModel::_METALIC_, filePath);
        delete filePath;
    }
    if (ambiantocclusion) {
        filePath = PathConcat(texturePath, ambiantocclusion);
        models_[modelType]->SetTexture(Rendering::IModel::_AMBIENTOCCLUSION_, filePath);
        delete filePath;
    }
    if (heightMap) {
        filePath = PathConcat(texturePath, heightMap);
        models_[modelType]->SetTexture(Rendering::IModel::_HEIGHT_, filePath);
        delete filePath;
    }
}
