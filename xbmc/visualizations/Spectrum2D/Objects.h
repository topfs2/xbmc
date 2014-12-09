#pragma once
#include <Eigen/Dense>
#include <vector>

#include "constants.h"

class Object3D
{
public:
  virtual ~Object3D() {
    for (unsigned int i = 0; i < children.size(); i++) {
      delete children[0]; // TODO Use shared_ptr?
    }
  }

  virtual CONSTANTS::Type type() { return CONSTANTS::Object3D; }

  Eigen::Vector3f position;
  Eigen::Vector3f rotation;
  Eigen::Vector3f scale;

  std::vector<Object3D *> children;
};

class Scene : public Object3D
{
public:
  virtual CONSTANTS::Type type() { return CONSTANTS::Scene; }
};

#include "Geometries.h"
#include "Materials.h"

class Mesh : public Object3D
{
public:
  Mesh(IGeometry *geometry, IMaterial *material) : geometry(geometry), material(material) { }
  virtual CONSTANTS::Type type() { return CONSTANTS::Mesh; }

  IGeometry *geometry;
  IMaterial *material;
};
