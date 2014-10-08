/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2000  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 *  Wed May 24 10:49:37 CDT 2000
 *  Fixes to threading/context creation for the nVidia X4 drivers by
 *  Christian Zander <phoenix@minion.de>
 */

/*
 *  Ported to XBMC by d4rk
 *  Also added 'hSpeed' to animate transition between bar heights
 *
 *  Ported to GLES 2.0 by Gimli
 */

#define __STDC_LIMIT_MACROS

#include "addons/include/xbmc_vis_dll.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <stdint.h>
#include <iostream>
#include <GL/glew.h>

static GLenum  g_mode = GL_FILL;

double PI = atan2(1, 1) * 4;
typedef double complex cplx;
 
void _fft(cplx buf[], cplx out[], int n, int step)
{
	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);
 
		for (int i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}
 
void fft(cplx buf[], int n)
{
	cplx out[n];
	for (int i = 0; i < n; i++) out[i] = buf[i];
 
	_fft(buf, out, n, 1);
}

#define AUDIO_BUFFER 512
#define NUM_BANDS (AUDIO_BUFFER / 2)

static GLfloat heights[NUM_BANDS];
static GLfloat scale;
static float hSpeed;

static void draw_rectangle(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2)
{
  if(y1 == y2)
  {
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y1, z1);
    glVertex3f(x2, y2, z2);

    glVertex3f(x2, y2, z2);
    glVertex3f(x1, y2, z2);
    glVertex3f(x1, y1, z1);
  }
  else
  {
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y1, z2);
    glVertex3f(x2, y2, z2);

    glVertex3f(x2, y2, z2);
    glVertex3f(x1, y2, z1);
    glVertex3f(x1, y1, z1);
  }
}

static void draw_bars(void)
{
  glClear(GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  
  glPolygonMode(GL_FRONT_AND_BACK, g_mode);
  glBegin(GL_TRIANGLES);

  GLfloat padding = 0.1;
  GLfloat xscale = (2.0 - padding * 2.0) / (NUM_BANDS * 1.0);
  GLfloat x = -1.0f + padding;

  glColor3f(0.2, 1.0, 0.2);
  for (unsigned int i = 0; i < NUM_BANDS; i++) {
    draw_rectangle(x, -0.9f, 0.0f, x + xscale * 0.5, heights[i] - 0.85f, 0.0f);
    x += xscale;
  }

  glEnd();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glPopMatrix();
}

//-- Create -------------------------------------------------------------------
// Called on load. Addon should fully initalize or return error status
//-----------------------------------------------------------------------------
ADDON_STATUS ADDON_Create(void* hdl, void* props)
{

  if (!props)
    return ADDON_STATUS_UNKNOWN;

  scale = 1.0 / log(256.0);
  hSpeed = 0.0;

  return ADDON_STATUS_NEED_SETTINGS;
}

//-- Render -------------------------------------------------------------------
// Called once per frame. Do all rendering here.
//-----------------------------------------------------------------------------
extern "C" void Render()
{
  glDisable(GL_BLEND);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-1, 1, -1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glPolygonMode(GL_FRONT, GL_FILL);

  draw_bars();

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
}

extern "C" void Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName)
{
  for(unsigned int x = 0; x < NUM_BANDS; x++)
  {
    heights[x] = 0.0;
  }
}

extern "C" void AudioData(const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength)
{
  cplx buf[iAudioDataLength];

  for (unsigned int i = 0; i < iAudioDataLength; i++) {
    buf[i] = pAudioData[i];
  }

  fft(buf, iAudioDataLength);

/*
  float fMinData = (float)iAudioDataLength * (float)iAudioDataLength * 3.0f / 8.0f * 0.5 * 0.5; // 3/8 for the Hann window, 0.5 as minimum amplitude
  float fInvMinData = 1.0f/fMinData;
*/

  float fInvMinData = 1.0f / (sqrt(iAudioDataLength) * 8.0);

  //float fInvMinData = 1.0f / iAudioDataLength;

  for(unsigned int i = 0; i < std::max(NUM_BANDS, iAudioDataLength); i++)
  {
    if (i > iAudioDataLength) {
      heights[i] = 0.0f;
    } else {
      heights[i] = cabs(buf[i]) * fInvMinData;
    }
  }
}


//-- GetInfo ------------------------------------------------------------------
// Tell XBMC our requirements
//-----------------------------------------------------------------------------
extern "C" void GetInfo(VIS_INFO* pInfo)
{
  pInfo->bWantsFreq = false;
  pInfo->iSyncDelay = 0;
}


//-- GetSubModules ------------------------------------------------------------
// Return any sub modules supported by this vis
//-----------------------------------------------------------------------------
extern "C" unsigned int GetSubModules(char ***names)
{
  return 0; // this vis supports 0 sub modules
}

//-- OnAction -----------------------------------------------------------------
// Handle XBMC actions such as next preset, lock preset, album art changed etc
//-----------------------------------------------------------------------------
extern "C" bool OnAction(long flags, const void *param)
{
  bool ret = false;
  return ret;
}

//-- GetPresets ---------------------------------------------------------------
// Return a list of presets to XBMC for display
//-----------------------------------------------------------------------------
extern "C" unsigned int GetPresets(char ***presets)
{
  return 0;
}

//-- GetPreset ----------------------------------------------------------------
// Return the index of the current playing preset
//-----------------------------------------------------------------------------
extern "C" unsigned GetPreset()
{
  return 0;
}

//-- IsLocked -----------------------------------------------------------------
// Returns true if this add-on use settings
//-----------------------------------------------------------------------------
extern "C" bool IsLocked()
{
  return false;
}

//-- Stop ---------------------------------------------------------------------
// This dll must cease all runtime activities
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Stop()
{
}

//-- Destroy ------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Destroy()
{
}

//-- HasSettings --------------------------------------------------------------
// Returns true if this add-on use settings
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" bool ADDON_HasSettings()
{
  return true;
}

//-- GetStatus ---------------------------------------------------------------
// Returns the current Status of this visualisation
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
}

//-- GetSettings --------------------------------------------------------------
// Return the settings for XBMC to display
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  return 0;
}

//-- FreeSettings --------------------------------------------------------------
// Free the settings struct passed from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------

extern "C" void ADDON_FreeSettings()
{
}

//-- SetSetting ---------------------------------------------------------------
// Set a specific Setting value (called from XBMC)
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void* value)
{
  if (!strSetting || !value)
    return ADDON_STATUS_UNKNOWN;

  if (strcmp(strSetting, "bar_height")==0)
  {
    switch (*(int*) value)
    {
    case 1://standard
      scale = 1.f / log(256.f);
      break;

    case 2://big
      scale = 2.f / log(256.f);
      break;

    case 3://real big
      scale = 3.f / log(256.f);
      break;

    case 4://unused
      scale = 0.33f / log(256.f);
      break;

    case 0://small
    default:
      scale = 0.5f / log(256.f);
      break;
    }
    return ADDON_STATUS_OK;
  }
  else if (strcmp(strSetting, "speed")==0)
  {
    switch (*(int*) value)
    {
    case 1:
      hSpeed = 0.025f;
      break;

    case 2:
      hSpeed = 0.0125f;
      break;

    case 3:
      hSpeed = 0.1f;
      break;

    case 4:
      hSpeed = 0.2f;
      break;

    case 0:
    default:
      hSpeed = 0.05f;
      break;
    }
    return ADDON_STATUS_OK;
  }
  else if (strcmp(strSetting, "mode")==0)
  {
#if defined(HAS_SDL_OPENGL)
    switch (*(int*) value)
    {
      case 1:
        g_mode = GL_LINE;
        break;

      case 2:
        g_mode = GL_POINT;
        break;

      case 0:
      default:
        g_mode = GL_FILL;
        break;
    }
#else
    switch (*(int*) value)
    {
      case 1:
        g_mode = GL_LINE_LOOP;
        break;

      case 2:
        g_mode = GL_LINES; //no points on gles!
        break;

      case 0:
      default:
        g_mode = GL_TRIANGLES;
        break;
    }

#endif

    return ADDON_STATUS_OK;
  }

  return ADDON_STATUS_UNKNOWN;
}

//-- Announce -----------------------------------------------------------------
// Receive announcements from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
}
