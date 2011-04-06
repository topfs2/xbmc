/*
 *      Copyright (C) 2005-2010 Team XBMC
 *      http://www.xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "PlayerOperations.h"
#include "Application.h"
#include "Util.h"
#include "PlayListPlayer.h"
#include "guilib/GUIWindowManager.h"
#include "interfaces/Builtins.h"
#include "PlayListPlayer.h"
#include "PartyModeManager.h"

using namespace Json;
using namespace JSONRPC;

JSON_STATUS CPlayerOperations::GetActivePlayers(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  Value ret(arrayValue);

  int activePlayers = GetActivePlayers();

  if (activePlayers & Video)
    ret.append("video");
  if (activePlayers & Audio)
    ret.append("audio");
  if (activePlayers & Picture)
    ret.append("picture");

  result.swap(ret);

  return OK;
}

JSON_STATUS CPlayerOperations::State(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      result["playing"] = g_application.IsPlaying();
      result["paused"] = g_application.IsPaused();
      result["partymode"] = g_partyModeManager.IsEnabled();

      return OK;
      break;

    case Picture:
      result["playing"] = true;
      result["paused"] = false;
      result["partymode"] = false;
      return OK;
      break;

    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::PlayPause(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      CBuiltins::Execute("playercontrol(play)");
      return ACK;
      break;

    case Picture:
      SendAction(ACTION_PAUSE);
      return ACK;
      break;

    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::Stop(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      g_application.getApplicationMessenger().SendAction(CAction(ACTION_STOP));
      return ACK;
      break;

    case Picture:
      SendAction(ACTION_STOP);
      return ACK;
      break;

    case None:
    default:
      return FailedToExecute;
      break;
  }
}



JSON_STATUS CPlayerOperations::SkipPrevious(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      g_application.getApplicationMessenger().SendAction(CAction(ACTION_PREV_ITEM));
      return ACK;
      break;

    case Picture:
      SendAction(ACTION_PREV_PICTURE);
      return ACK;
      break;

    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::SkipNext(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      g_application.getApplicationMessenger().SendAction(CAction(ACTION_NEXT_ITEM));
      return ACK;
      break;

    case Picture:
      SendAction(ACTION_NEXT_PICTURE);
      return ACK;
      break;

    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::BigSkipBackward(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      CBuiltins::Execute("playercontrol(bigskipbackward)");
      return ACK;
      break;

    case Picture:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::BigSkipForward(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      CBuiltins::Execute("playercontrol(bigskipforward)");
      return ACK;
      break;

    case Picture:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::SmallSkipBackward(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      CBuiltins::Execute("playercontrol(smallskipbackward)");
      return ACK;
      break;

    case Picture:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::SmallSkipForward(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      CBuiltins::Execute("playercontrol(smallskipforward)");
      return ACK;
      break;

    case Picture:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::Rewind(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      g_application.getApplicationMessenger().SendAction(CAction(ACTION_PLAYER_REWIND));
      return ACK;
      break;

    case Picture:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::Forward(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      g_application.getApplicationMessenger().SendAction(CAction(ACTION_PLAYER_FORWARD));
      return ACK;
      break;

    case Picture:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::MoveLeft(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Picture:
      SendAction(ACTION_MOVE_LEFT);
      return ACK;
      break;

    case Video:
    case Audio:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::MoveRight(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Picture:
      SendAction(ACTION_MOVE_RIGHT);
      return ACK;
      break;

    case Video:
    case Audio:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::MoveDown(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Picture:
      SendAction(ACTION_MOVE_DOWN);
      return ACK;
      break;

    case Video:
    case Audio:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::MoveUp(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Picture:
      SendAction(ACTION_MOVE_UP);
      return ACK;
      break;

    case Video:
    case Audio:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::ZoomOut(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Picture:
      SendAction(ACTION_ZOOM_OUT);
      return ACK;
      break;

    case Video:
    case Audio:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::ZoomIn(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Picture:
      SendAction(ACTION_ZOOM_IN);
      return ACK;
      break;

    case Video:
    case Audio:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::Zoom(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Picture:
      SendAction(ACTION_ZOOM_LEVEL_NORMAL + (parameterObject["value"].asInt() - 1));
      return ACK;
      break;

    case Video:
    case Audio:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::Rotate(const CStdString &method, ITransportLayer *transport, IClient *client, const Value &parameterObject, Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Picture:
      SendAction(ACTION_ROTATE_PICTURE);
      return ACK;
      break;

    case Video:
    case Audio:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::GetTime(const CStdString &method, ITransportLayer *transport, IClient *client, const Json::Value& parameterObject, Json::Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      CreateTime((int)(g_application.GetTime() * 1000.0), result["time"]);
      CreateTime((int)(g_application.GetTotalTime() * 1000.0), result["total"]);
      result["playing"] = g_application.IsPlaying();
      result["paused"] = g_application.IsPaused();
      return OK;
      break;

    case Picture:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::GetPercentage(const CStdString &method, ITransportLayer *transport, IClient *client, const Json::Value& parameterObject, Json::Value &result)
{
  Value val;
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      val = g_application.GetPercentage();
      result.swap(val);
      return OK;
      break;

    case Picture:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::SeekTime(const CStdString &method, ITransportLayer *transport, IClient *client, const Json::Value& parameterObject, Json::Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      g_application.SeekTime(parameterObject["value"].asInt());
      return ACK;
      break;

    case Picture:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}

JSON_STATUS CPlayerOperations::SeekPercentage(const CStdString &method, ITransportLayer *transport, IClient *client, const Json::Value& parameterObject, Json::Value &result)
{
  switch (PlayerObjectType(parameterObject["player"]))
  {
    case Video:
    case Audio:
      g_application.SeekPercentage(parameterObject["value"].asFloat());
      return ACK;
      break;

    case Picture:
    case None:
    default:
      return FailedToExecute;
      break;
  }
}




int CPlayerOperations::GetActivePlayers()
{
  int activePlayers = 0;

  if (g_application.IsPlayingVideo())
    activePlayers |= Video;
  if (g_application.IsPlayingAudio())
    activePlayers |= Audio;
  if (g_windowManager.IsWindowActive(WINDOW_SLIDESHOW))
    activePlayers |= Picture;

  return activePlayers;
}

PlayerType CPlayerOperations::PlayerObjectType(const Json::Value &player)
{
  int activePlayers = GetActivePlayers();
  int playerID = PlayerImplicit;

  if (player.isString())
  {
    CStdString givenID = player.asCString();

    if (givenID.Equals("video"))
      playerID = Video;
    else if (givenID.Equals("audio"))
      playerID = Audio;
    else if (givenID.Equals("picture"))
      playerID = Picture;
  }

  int choosenPlayer = playerID & activePlayers;

  // Implicit order
  if (choosenPlayer & Video)
    return Video;
  else if (choosenPlayer & Audio)
    return Audio;
  else if (choosenPlayer & Picture)
    return Picture;
  else
    return None;
}

void CPlayerOperations::SendAction(int actionID)
{
  g_application.getApplicationMessenger().SendAction(CAction(actionID), WINDOW_SLIDESHOW);
}

void CPlayerOperations::CreateTime(int time, Json::Value &result)
{
  int ms = time % 1000;
  result["milliseconds"] = ms;
  time = (time - ms) / 1000;

  int s = time % 60;
  result["seconds"] = s;
  time = (time - s) / 60;

  int m = time % 60;
  result["minutes"] = m;
  time = (time -m) / 60;

  result["hours"] = time;
}
