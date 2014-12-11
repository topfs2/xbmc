#include "RenderProgram.h"
#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <cstdarg>

using namespace std;

GLuint active_shader_program = 0;

bool checkOpenGLError(const char *file, int line, const char *format, ...)
{
  return false;

  GLenum glErr;
  bool error = false;

  glErr = glGetError();
  while (glErr != GL_NO_ERROR)
  {
    cerr << "glError in file " << file << " @ line " << line << ": " << gluErrorString(glErr) << endl;
    if (format != NULL)
    {
      va_list va;
      va_start(va, format);

      printf(format, va);
      printf("\n");
      va_end(va);
    }

    error = true;
    glErr = glGetError();
  }
  return error;
}

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = new char[infologLength];
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        delete [] infoLog;
    }
}

void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = new char[infologLength];
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        delete [] infoLog;
    }
}

CRenderProgram::CRenderProgram(const string &vertexShaderSrc, const string &fragmentShaderSrc)
{
  m_shaderProgram = compileAndLink(vertexShaderSrc.c_str(), fragmentShaderSrc.c_str());

  revertToFixedPipeline();
}

CRenderProgram::~CRenderProgram()
{
  if (m_vs)
  {
    glDetachShader(m_shaderProgram, m_vs);
    glDeleteShader(m_vs);
  }

  if (m_fs)
  {
    glDetachShader(m_shaderProgram, m_fs);
    glDeleteShader(m_fs);
  }

  if (m_shaderProgram)
    glDeleteProgram(m_shaderProgram);

  for (UNIFORM_MAP::iterator itr = m_uniforms.begin(); itr != m_uniforms.end(); itr++)
    delete itr->second.data;
}

void CRenderProgram::Bind()
{
  glUseProgram(m_shaderProgram);
  active_shader_program = m_shaderProgram;

  for (UNIFORM_MAP::iterator itr = m_uniforms.begin(); itr != m_uniforms.end(); itr++)
    bindUniform(itr->second);
}

void CRenderProgram::revertToFixedPipeline()
{
  glUseProgram(0);
  active_shader_program = 0;
}

void CRenderProgram::uniform1i(const char *name, GLint v0)
{
  uniformData *data = new uniformData();
  data->i = v0;
  bindUniform(name, UNIFORM_1_I, data);
}

void CRenderProgram::uniform1f(const char *name, GLfloat v0)
{
  uniformData *ud = new uniformData();
  ud->f = v0;
  bindUniform(name, UNIFORM_1_F, ud);
}

void CRenderProgram::uniform3f(const char *name, GLfloat v0, GLfloat v1, GLfloat v2)
{
  uniformData *data = new uniformData();
  data->fff = {v0, v1, v2};
  bindUniform(name, UNIFORM_3_F, data);
}

void CRenderProgram::uniform4f(const char *name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
  uniformData *data = new uniformData();
  data->ffff = {v0, v1, v2, v3};
  bindUniform(name, UNIFORM_4_F, data);
}

GLuint CRenderProgram::compile(GLenum shaderType, const char *shader)
{
  assert(shader != NULL);

  GLuint s = glCreateShader(shaderType);
  if (s == 0 || checkOpenGLError(__FILE__, __LINE__, NULL))
  {
    cerr << "Failed to create shader from\n====" << endl;
    cerr << shader << endl;
    cerr << "===" << endl;

    return 0;
  }

  glShaderSource(s, 1, &shader, NULL);
  if (checkOpenGLError(__FILE__, __LINE__, NULL))
  {
    cerr << "Failed to add shader source\n====" << endl;
    cerr << shader << endl;
    cerr << "===" << endl;
    glDeleteShader(s);

    return 0;
  }

  glCompileShader(s);

  printShaderInfoLog(s);

  GLint param;
  glGetShaderiv(s, GL_COMPILE_STATUS, &param);
  if (param != GL_TRUE)
  {
    cerr << "Failed to compile shader source\n====" << endl;
    cerr << shader << endl;
    cerr << "===" << endl;
    glDeleteShader(s);

    return 0;
  }

  if (checkOpenGLError(__FILE__, __LINE__, NULL))
  {
    cerr << "Failed to compile shader source\n====" << endl;
    cerr << shader << endl;
    cerr << "===" << endl;
    glDeleteShader(s);

    return 0;
  }

  return s;
}

GLuint CRenderProgram::compileAndLink(const char *vertexShader, const char *fragmentShader)
{
  assert(vertexShader != NULL);
  assert(fragmentShader != NULL);

  GLuint program = glCreateProgram();
  if (program == 0 || checkOpenGLError(__FILE__, __LINE__, "Failed to create program"))
    return 0;

  GLuint vs = compile(GL_VERTEX_SHADER, vertexShader);
  GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentShader);

  if (vs && fs)
  {
    glAttachShader(program, vs);
    if (checkOpenGLError(__FILE__, __LINE__, "Failed to attach vertex shader source"))
    {
      glDeleteShader(vs);
      glDeleteShader(fs);
      glDeleteProgram(program);
      return 0;
    }

    glAttachShader(program, fs);
    if (checkOpenGLError(__FILE__, __LINE__, "Failed to attach fragment shader source"))
    {
      glDetachShader(program, vs);
      glDeleteShader(vs);

      glDeleteShader(fs);

      glDeleteProgram(program);
      return 0;
    }

    glLinkProgram(program);
    if (checkOpenGLError(__FILE__, __LINE__, "Failed to link shader program"))
    {
      glDetachShader(program, vs);
      glDeleteShader(vs);

      glDetachShader(program, fs);
      glDeleteShader(fs);

      glDeleteProgram(program);
      return 0;
    }

    GLint param;
    glGetProgramiv(program, GL_LINK_STATUS, &param);
    if (param != GL_TRUE)
    {
      cerr << "Failed to link shader program" << endl;
      glDetachShader(program, vs);
      glDeleteShader(vs);

      glDetachShader(program, fs);
      glDeleteShader(fs);

      glDeleteProgram(program);
      return 0;
    }

    printProgramInfoLog(program);

    glUseProgram(program);

    m_vs = vs;
    m_fs = fs;
    m_shaderProgram = program;
    return program;
  }
  cerr << "Failed to create program" << endl;

  if (vs)
    glDeleteShader(vs);

  if (fs)
    glDeleteShader(fs);

  checkOpenGLError(__FILE__, __LINE__, NULL);
  return 0;
}

void CRenderProgram::bindUniform(const uniform &u)
{
  if (active_shader_program != m_shaderProgram)
    return;

  switch (u.type)
  {
    case UNIFORM_1_I:
      glUniform1i(u.loc, u.data->i);
      break;
    case UNIFORM_1_F:
      glUniform1f(u.loc, u.data->f);
      break;
    case UNIFORM_3_F:
      glUniform3f(u.loc, u.data->fff.v0, u.data->fff.v1, u.data->fff.v2);
      break;
    case UNIFORM_4_F:
      glUniform4f(u.loc, u.data->ffff.v0, u.data->ffff.v1, u.data->ffff.v2, u.data->ffff.v3);
      break;

  }
}

void CRenderProgram::bindUniform(const char *name, uniformType type, uniformData *data)
{
  UNIFORM_MAP::iterator itr = m_uniforms.find(name);
  if (itr == m_uniforms.end())
  {
    uniform u;
    u.loc = glGetUniformLocation(m_shaderProgram, name);
    u.type = type;
    u.data = data;

    m_uniforms[name] = u;

    bindUniform(u);
  }
  else
  {
    // Just update
    delete itr->second.data;

    itr->second.type = type;
    itr->second.data = data;

    bindUniform(itr->second);
  }
}
