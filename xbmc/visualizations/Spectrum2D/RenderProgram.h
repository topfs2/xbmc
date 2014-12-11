#pragma once
#include <GL/glew.h>

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

enum uniformType
{
  UNIFORM_1_I,
  UNIFORM_1_F,
  UNIFORM_3_F,
  UNIFORM_4_F
};

struct threeF
{
  GLfloat v0, v1, v2;
};

struct fourF
{
  GLfloat v0, v1, v2, v3;
};

union uniformData
{
  GLint i;
  GLfloat f;
  threeF fff;
  fourF ffff;
};

struct uniform
{
  GLint       loc;
  uniformType type;
  uniformData *data;
};

class CRenderProgram
{
public:
  CRenderProgram(const std::string &vertexShaderSrc, const std::string &fragmentShaderSrc);
  ~CRenderProgram();

  void Bind();
  static void revertToFixedPipeline();

  void uniform1i(const char *name, GLint v0);
  void uniform1f(const char *name, GLfloat v0);
  void uniform3f(const char *name, GLfloat v0, GLfloat v1, GLfloat v2);
  void uniform4f(const char *name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

private:
  char *readFile(const char *filepath);

  GLuint compile(GLenum type, const char *shader);
  GLuint compileAndLink(const char *vertexShader, const char *fragmentShader);

  void bindUniform(const uniform &u);
  void bindUniform(const char *name, uniformType type, uniformData *data);

  GLuint m_vs;
  GLuint m_fs;
  GLuint m_shaderProgram;

  typedef std::map<std::string, uniform> UNIFORM_MAP;

  UNIFORM_MAP m_uniforms;
};

typedef boost::shared_ptr<CRenderProgram> CRenderProgramPtr;
