/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "system.h"

#include "GLUtils.h"
#include "log.h"
#include "settings/AdvancedSettings.h"
#include "windowing/WindowingFactory.h"

using namespace log4cplus;

static Logger logger = Logger::getInstance("rendering.OpenGL");

void _VerifyGLState(const char* szfile, const char* szfunction, int lineno){
#if defined(HAS_GL) && defined(_DEBUG)
#define printMatrix(matrix)                                         \
  {                                                                 \
    for (int ixx = 0 ; ixx<4 ; ixx++)                               \
      {                                                             \
        LOG4CPLUS_DEBUG(logger, matrix[ixx*4]   << " " <<           \
                                matrix[ixx*4+1] << " " <<           \
                                matrix[ixx*4+2] << " " <<           \
                                matrix[ixx*4+3]);                   \
      }                                                             \
  }
  GLenum err = glGetError();
  if (err==GL_NO_ERROR)
    return;
  LOG4CPLUS_ERROR(logger, "GL ERROR: " << gluErrorString(err));
  if (szfile && szfunction)
      LOG4CPLUS_ERROR(logger, "In file: " << szfile << " function: " << szfunction << " line: " << lineno);
  GLboolean bools[16];
  GLfloat matrix[16];
  glGetFloatv(GL_SCISSOR_BOX, matrix);
  LOG4CPLUS_DEBUG(logger, "Scissor box: " << matrix[0] << ", " << matrix[1] << ", " << matrix[2] << ", " << matrix[3]);
  glGetBooleanv(GL_SCISSOR_TEST, bools);
  LOG4CPLUS_DEBUG(logger, "Scissor test enabled: " << (int)bools[0]);
  glGetFloatv(GL_VIEWPORT, matrix);
  LOG4CPLUS_DEBUG(logger, "Viewport: " << matrix[0] << ", " << matrix[1] << ", " << matrix[2] << ", " << matrix[3]);
  glGetFloatv(GL_PROJECTION_MATRIX, matrix);
  LOG4CPLUS_DEBUG(logger, "Projection Matrix:");
  printMatrix(matrix);
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
  LOG4CPLUS_DEBUG(logger, "Modelview Matrix:");
  printMatrix(matrix);
//  abort();
#endif
}

void LogGraphicsInfo()
{
#if defined(HAS_GL) || defined(HAS_GLES)
  const GLubyte *s;

  s = glGetString(GL_VENDOR);
  if (s)
    LOG4CPLUS_INFO(logger, "GL_VENDOR = " << s);
  else
    LOG4CPLUS_INFO(logger, "GL_VENDOR = NULL");

  s = glGetString(GL_RENDERER);
  if (s)
    LOG4CPLUS_INFO(logger, "GL_RENDERER = " << s);
  else
    LOG4CPLUS_INFO(logger, "GL_RENDERER = NULL");

  s = glGetString(GL_VERSION);
  if (s)
    LOG4CPLUS_INFO(logger, "GL_VERSION = " << s);
  else
    LOG4CPLUS_INFO(logger, "GL_VERSION = NULL");

  s = glGetString(GL_SHADING_LANGUAGE_VERSION);
  if (s)
    LOG4CPLUS_INFO(logger, "GL_SHADING_LANGUAGE_VERSION = " << s);
  else
    LOG4CPLUS_INFO(logger, "GL_SHADING_LANGUAGE_VERSION = NULL");

  //GL_NVX_gpu_memory_info extension
#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B

  if (g_Windowing.IsExtSupported("GL_NVX_gpu_memory_info"))
  {
    GLint mem = 0;

    glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &mem);
    LOG4CPLUS_INFO(logger, "GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX = " << mem);

    //this seems to be the amount of ram on the videocard
    glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &mem);
    LOG4CPLUS_INFO(logger, "GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX = " << mem);
  }

  s = glGetString(GL_EXTENSIONS);
  if (s)
    LOG4CPLUS_INFO(logger, "GL_EXTENSIONS = " << s);
  else
    LOG4CPLUS_INFO(logger, "GL_EXTENSIONS = NULL");

#else /* !HAS_GL */
  LOG4CPLUS_INFO(logger,
            "Please define LogGraphicsInfo for your chosen graphics libary");
#endif /* !HAS_GL */
}

int glFormatElementByteCount(GLenum format)
{
  switch (format)
  {
#ifndef HAS_GLES
  case GL_BGRA:
#endif
  case GL_RGBA:
    return 4;
#ifndef HAS_GLES
  case GL_BGR:
#endif
  case GL_RGB:
    return 3;
  case GL_LUMINANCE_ALPHA:
    return 2;
  case GL_LUMINANCE:
  case GL_ALPHA:
    return 1;
  default:
    LOG4CPLUS_ERROR(logger, "glFormatElementByteCount - Unknown format " << format);
    return 1;
  }
}
