/******************************************************************************
    * Author : Cody Morgan
    * Class  : GAM 300
    * Brief  : Mesh data for drawing objects

    (c) DigiPen Institute of Technology 2019
******************************************************************************/

#include "pch.h"
#include "Mesh.h"
using std::vector;

const double pi = 3.14159265358979323846;


Mesh::Mesh(MeshType type) : meshType_(type)
{
    switch (type)
    {   
    case MeshType::Cube:
        loadCube();
        break;

    case MeshType::Sphere:
        loadSphere();
        break;
    }
}

void Mesh::loadCube(float scale)
{
    vertices_ = 
    {
         scale * 0.5f,  scale * 0.5f, -scale * 0.5f,
         scale * 0.5f, -scale * 0.5f, -scale * 0.5f,
        -scale * 0.5f, -scale * 0.5f, -scale * 0.5f,
        -scale * 0.5f,  scale * 0.5f, -scale * 0.5f,
         scale * 0.5f,  scale * 0.5f,  scale * 0.5f,
         scale * 0.5f, -scale * 0.5f,  scale * 0.5f,
        -scale * 0.5f, -scale * 0.5f,  scale * 0.5f,
        -scale * 0.5f,  scale * 0.5f,  scale * 0.5f,
    };

    indices_ = 
    {
        4, 0, 3,
        4, 3, 7,
        2, 6, 7,
        2, 7, 3,
        1, 5, 2,
        5, 6, 2,
        0, 4, 1,
        4, 5, 1,
        4, 7, 5,
        7, 6, 5,
        0, 1, 2,
        0, 2, 3
    };
}

void Mesh::loadSphere(float radius, int divisions)
{
    vertices_.clear();
    indices_.clear();
    vector<float>position = {0,0,0};

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
        position[2] = radius * sinf(theta);

        for (int j = 0; j <= divisions; ++j)
        {
            phi = j * sectorStep;
            position[0] = xy * cosf(phi);
            position[1] = xy * sinf(phi);
        }
    }

    // generate indices
    unsigned k1;
    unsigned k2;
    for (int i = 0; i < divisions; ++i)
    {
        k1 = i * (divisions + 1);     // beginning of current stack
        k2 = k1 + divisions + 1;      // beginning of next stack

        for (int j = 0; j < divisions; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices_.push_back(k1);
                indices_.push_back(k2);
                indices_.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (divisions - 1))
            {
                indices_.push_back(k1 + 1);
                indices_.push_back(k2);
                indices_.push_back(k2 + 1);
            }
        }
    }
}

void Mesh::loadMesh(const std::vector<float>& verts, const std::vector<unsigned>& indices)
{
    vertices_ = verts;
    indices_ = indices;
}

void Mesh::update()
{
    // send vert and indices to graphics
}


