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

#include "addons/include/xbmc_vis_dll.h"
#include "RenderProgram.h"
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <SDL/SDL.h> // Grrr
#include <math.h>
#include <complex.h>

using namespace std;

void LogProps(VIS_PROPS *props)
{
  cout << "Props = {" << endl
       << "\t x: " << props->x << endl
       << "\t y: " << props->y << endl
       << "\t width: " << props->width << endl
       << "\t height: " << props->height << endl
       << "\t pixelRatio: " << props->pixelRatio << endl
       << "\t name: " << props->name << endl
       << "\t presets: " << props->presets << endl
       << "\t profile: " << props->profile << endl
//       << "\t submodule: " << props->submodule << endl // Causes problems? Is it initialized?
       << "}" << endl;
}

void LogTrack(VisTrack *track)
{
  cout << "Track = {" << endl
       << "\t title: " << track->title << endl
       << "\t artist: " << track->artist << endl
       << "\t album: " << track->album << endl
       << "\t albumArtist: " << track->albumArtist << endl
       << "\t genre: " << track->genre << endl
       << "\t comment: " << track->comment << endl
       << "\t lyrics: " << track->lyrics << endl
       << "\t trackNumber: " << track->trackNumber << endl
       << "\t discNumber: " << track->discNumber << endl
       << "\t duration: " << track->duration << endl
       << "\t year: " << track->year << endl
       << "\t rating: " << track->rating << endl
       << "}" << endl;
}

void LogAction(const char *message)
{
  cout << "Action " << message << endl;
}

void LogActionString(const char *message, const char *param)
{
  cout << "Action " << message << " " << param << endl;
}

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

GLuint createTexture(GLint format, unsigned int w, unsigned int h, const GLvoid * data)
{
  GLuint texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
  return texture;
}

std::string header_fs =
"uniform vec3      iResolution;\n"
"uniform float     iGlobalTime;\n"
"uniform float     iChannelTime[4];\n"
"uniform vec4      iMouse;\n"
"uniform vec4      iDate;\n"
"uniform float     iSampleRate;\n"
"uniform vec3      iChannelResolution[4];\n"
"uniform sampler2D iChannel0;\n"
"uniform sampler2D iChannel1;\n"
"uniform sampler2D iChannel2;\n"
"uniform sampler2D iChannel3;\n";

//-- Globals
std::string sample_vs = "void main() { gl_Position = ftransform(); }";
std::string sample_fs = header_fs + "\n"
"#define WAVES 8.0\n"
"void main(void) {\n"
"	vec2 uv = -1.0 + 2.0 * gl_FragCoord.xy / iResolution.xy;\n"
"	float time = iGlobalTime * 1.0;\n"
"	vec3 color = vec3(0.0);\n"
"	for (float i=0.0; i<WAVES + 1.0; i++) {\n"
"		float freq = texture2D(iChannel0, vec2(i / WAVES, 0.0)).x * 7.0;\n"
"		vec2 p = vec2(uv);\n"
"		p.x += i * 0.04 + freq * 0.03;\n"
"		p.y += sin(p.x * 10.0 + time) * cos(p.x * 2.0) * freq * 0.2 * ((i + 1.0) / WAVES);\n"
"		float intensity = abs(0.01 / p.y) * clamp(freq, 0.35, 2.0);\n"
"		color += vec3(1.0 * intensity * (i / 5.0), 0.5 * intensity, 1.75 * intensity) * (3.0 / WAVES);\n"
"	}\n"
"	gl_FragColor = vec4(color, 1.0);\n"
"}";

bool initialized = false;
CRenderProgram *shader = NULL;
GLuint iChannel0 = 0;

char *audio_data = NULL;
int samplesPerSec = 0;

//-- Render -------------------------------------------------------------------
// Called once per frame. Do all rendering here.
//-----------------------------------------------------------------------------
extern "C" void Render()
{
  if (initialized) {
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, iChannel0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, NUM_BANDS, 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, audio_data);

    shader->Bind();
    shader->uniform1f("iGlobalTime", (float)SDL_GetTicks() / 1000.0f);
    shader->uniform1f("iSampleRate", samplesPerSec);

    glBegin(GL_QUADS);                      // Draw A Quad
        glVertex3f(-1.0f, 1.0f, 0.0f);              // Top Left
        glVertex3f( 1.0f, 1.0f, 0.0f);              // Top Right
        glVertex3f( 1.0f,-1.0f, 0.0f);              // Bottom Right
        glVertex3f(-1.0f,-1.0f, 0.0f);              // Bottom Left
    glEnd();                            // Done Drawing The Quad

    CRenderProgram::revertToFixedPipeline();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
  }
}

extern "C" void Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName)
{
  cout << "Start " << iChannels << " " << iSamplesPerSec << " " << iBitsPerSample << " " << szSongName << std::endl;
  samplesPerSec = iSamplesPerSec;
}

extern "C" void AudioData(const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength)
{
  cplx buf[iAudioDataLength];

  for (unsigned int i = 0; i < iAudioDataLength; i++) {
    double multiplier = 0.5 * (1.0 - cos(2 * PI * i / (iAudioDataLength - 1)));
    buf[i] = pAudioData[i] * multiplier;
  }

  fft(buf, iAudioDataLength);

  float fInvMinData = 1.0f / (sqrt(iAudioDataLength) * 1.0);

  for(unsigned int i = 0; i < NUM_BANDS; i++)
  {
    double value = cabs(buf[i]) * fInvMinData * 256.0;
    audio_data[i] = (char)value;
  }
}

//-- GetInfo ------------------------------------------------------------------
// Tell XBMC our requirements
//-----------------------------------------------------------------------------
extern "C" void GetInfo(VIS_INFO *pInfo)
{
  cout << "GetInfo" << std::endl;
  pInfo->bWantsFreq = false;
  pInfo->iSyncDelay = 0;
}


//-- GetSubModules ------------------------------------------------------------
// Return any sub modules supported by this vis
//-----------------------------------------------------------------------------
extern "C" unsigned int GetSubModules(char ***names)
{
  cout << "GetSubModules" << std::endl;
  return 0; // this vis supports 0 sub modules
}

//-- OnAction -----------------------------------------------------------------
// Handle XBMC actions such as next preset, lock preset, album art changed etc
//-----------------------------------------------------------------------------
extern "C" bool OnAction(long flags, const void *param)
{
  switch (flags)
  {
    case VIS_ACTION_NEXT_PRESET:
      LogAction("VIS_ACTION_NEXT_PRESET");
      break;
    case VIS_ACTION_PREV_PRESET:
      LogAction("VIS_ACTION_PREV_PRESET");
      break;
    case VIS_ACTION_LOAD_PRESET:
      LogAction("VIS_ACTION_LOAD_PRESET"); // TODO param is int *
      break;
    case VIS_ACTION_RANDOM_PRESET:
      LogAction("VIS_ACTION_RANDOM_PRESET");
      break;
    case VIS_ACTION_LOCK_PRESET:
      LogAction("VIS_ACTION_LOCK_PRESET");
      break;
    case VIS_ACTION_RATE_PRESET_PLUS:
      LogAction("VIS_ACTION_RATE_PRESET_PLUS");
      break;
    case VIS_ACTION_RATE_PRESET_MINUS:
      LogAction("VIS_ACTION_RATE_PRESET_MINUS");
      break;
    case VIS_ACTION_UPDATE_ALBUMART:
      LogActionString("VIS_ACTION_UPDATE_ALBUMART", (const char *)param);
      break;
    case VIS_ACTION_UPDATE_TRACK:
      LogTrack((VisTrack *)param);
      break;

    default:
      break;
  }

  return false;
}

//-- GetPresets ---------------------------------------------------------------
// Return a list of presets to XBMC for display
//-----------------------------------------------------------------------------
extern "C" unsigned int GetPresets(char ***presets)
{
  cout << "GetPresets" << std::endl;
  return 0;
}

//-- GetPreset ----------------------------------------------------------------
// Return the index of the current playing preset
//-----------------------------------------------------------------------------
extern "C" unsigned GetPreset()
{
  cout << "GetPreset" << std::endl;
  return 0;
}

//-- IsLocked -----------------------------------------------------------------
// Returns true if this add-on use settings
//-----------------------------------------------------------------------------
extern "C" bool IsLocked()
{
  cout << "IsLocked" << std::endl;
  return false;
}

//-- Create -------------------------------------------------------------------
// Called on load. Addon should fully initalize or return error status
//-----------------------------------------------------------------------------
ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  cout << "ADDON_Create" << std::endl;
  VIS_PROPS *p = (VIS_PROPS *)props;

  LogProps(p);

  audio_data = new char[NUM_BANDS * 2];
  memset(audio_data, 0, NUM_BANDS * 2);

  if (GLEW_OK != glewInit()) {
	  std::cout << "Failed to initialize glew";
  }

  if (!initialized)
  {
    cout << "VertexShader: " << endl << sample_vs << endl;
    cout << "FragmentShader: " << endl << sample_fs << endl;

    shader = new CRenderProgram(sample_vs, sample_fs);

    iChannel0 = createTexture(GL_LUMINANCE, NUM_BANDS, 2, audio_data);

    shader->uniform3f("iResolution", p->width, p->height, 0.0f);
    shader->uniform1f("iChannel0", 0);

    initialized = true;
  }

  if (!props)
    return ADDON_STATUS_UNKNOWN;

  return ADDON_STATUS_NEED_SETTINGS;
}

//-- Stop ---------------------------------------------------------------------
// This dll must cease all runtime activities
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Stop()
{
  cout << "ADDON_Stop" << std::endl;
}

//-- Destroy ------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Destroy()
{
  cout << "ADDON_Destroy" << std::endl;

  if (shader) {
    delete shader;
    shader = NULL;
  }

  if (iChannel0) {
    glDeleteTextures(1, &iChannel0);
    iChannel0 = 0;
  }

  if (audio_data) {
    delete [] audio_data;
    audio_data = NULL;
  }

  initialized = false;
}

//-- HasSettings --------------------------------------------------------------
// Returns true if this add-on use settings
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" bool ADDON_HasSettings()
{
  cout << "ADDON_HasSettings" << std::endl;
  return false;
}

//-- GetStatus ---------------------------------------------------------------
// Returns the current Status of this visualisation
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" ADDON_STATUS ADDON_GetStatus()
{
  cout << "ADDON_GetStatus" << std::endl;
  return ADDON_STATUS_OK;
}

//-- GetSettings --------------------------------------------------------------
// Return the settings for XBMC to display
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  cout << "ADDON_GetSettings" << std::endl;
  return 0;
}

//-- FreeSettings --------------------------------------------------------------
// Free the settings struct passed from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------

extern "C" void ADDON_FreeSettings()
{
  cout << "ADDON_FreeSettings" << std::endl;
}

//-- SetSetting ---------------------------------------------------------------
// Set a specific Setting value (called from XBMC)
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void* value)
{
  cout << "ADDON_SetSetting " << strSetting << std::endl;
  if (!strSetting || !value)
    return ADDON_STATUS_UNKNOWN;

/*
  if (strcmp(strSetting, "foo") == 0)
  {
    return ADDON_STATUS_OK;
  }
*/

  return ADDON_STATUS_UNKNOWN;
}

//-- Announce -----------------------------------------------------------------
// Receive announcements from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
  cout << "ADDON_Announce " << flag << " " << sender << " " << message << std::endl;
}
