#pragma once

class IGeometry
{
public:
  virtual ~IGeometry() { }
};

class PlaneGeometry : public IGeometry
{
public:
  PlaneGeometry(float width, float height, float widthSegments, float heightSegments) : width(width), height(height), widthSegments(widthSegments), heightSegments(heightSegments) { }

  float width;
  float height;
  float widthSegments;
  float heightSegments;
};
