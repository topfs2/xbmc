#pragma once

class IMaterial
{
public:
  virtual ~IMaterial() { }
};

class ShaderMaterial : public IMaterial
{
public:
  ShaderMaterial(const std::string &vertexShader, const std::string &fragmentShader) : vertexShader(vertexShader), fragmentShader(fragmentShader) { }

  std::string vertexShader;
  std::string fragmentShader;
};
