#pragma once

class ICamera
{
public:
  virtual ~ICamera() { }

};

class OrthographicCamera : public ICamera
{
public:
  OrthographicCamera(float width, float height, float near, float far) :  left(width / -2.0f), right(width / 2.0f), top(height / 2.0f), bottom(height / -2.0f), near(near), far(far) { }
  OrthographicCamera(float left, float right, float top, float bottom, float near, float far) : left(left), right(right), top(top), bottom(bottom), near(near), far(far) { }

  float left;
  float right;
  float top;
  float bottom;
  float near;
  float far;
};
