/******************************************************************************
  * Author : Cody Morgan
  * Class  : GAM 300
  * Brief  : Mesh data for drawing objects

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once
#ifndef MESH_H
#define MESH_H

#include <vector>


enum class MeshType
{
  None,
  Cube,
  Sphere,
  Cylinder,
  Cone,
};

class Mesh
{
public:
  Mesh(MeshType type = MeshType::Cube);


  /////***** Mesh Manipulation *****/////


  // load mesh primative using class enum above
  void loadCube(float scale = 1);
  void loadSphere(float radius = 1.0f, int divisions = 40);

  // load mesh data using your own vertex information
  void loadMesh(const std::vector<float>& verts, const std::vector<unsigned>& indices);


  /////***** Other Functionality *****/////


  void update();

private:
  std::vector<float> vertices_;
  std::vector<unsigned> indices_;
  MeshType meshType_ = MeshType::None;
};


#endif