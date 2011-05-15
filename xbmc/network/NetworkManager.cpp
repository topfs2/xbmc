/*
 *      Copyright (C) 2005-2011 Team XBMC
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

#include "NetworkManager.h"
#include "NullNetworkManager.h"
#include "linux/ConnmanNetworkManager.h"
#include "utils/log.h"
#include "utils/RssReader.h"
#include "libscrobbler/lastfmscrobbler.h"
#include "libscrobbler/librefmscrobbler.h"
#include "Application.h"
#include "ApplicationMessenger.h"

using namespace std;

CNetworkManager::CNetworkManager()
{
  m_instance = NULL;
  m_state = NETWORK_CONNECTION_STATE_UNKNOWN;
}

CNetworkManager::~CNetworkManager()
{
  delete m_instance;
}

void CNetworkManager::Initialize()
{
#ifdef HAS_DBUS
  if (CConnmanNetworkManager::HasConnman())
    m_instance = new CConnmanNetworkManager();
#endif

  if (m_instance == NULL)
    m_instance = new CNullNetworkManager();

  m_defaultConnection = CConnectionPtr(new CNullConnection());
  OnConnectionListChange(m_instance->GetConnections());
}

void CNetworkManager::PumpNetworkEvents()
{
  m_instance->PumpNetworkEvents(this);
}

string CNetworkManager::GetDefaultConnectionName()
{
  return m_defaultConnection->GetName();
}

string CNetworkManager::GetDefaultConnectionIP()
{
  return m_defaultConnection->GetIP();
}

std::string CNetworkManager::GetDefaultConnectionNetmask()
{
  return m_defaultConnection->GetNetmask();
}

std::string CNetworkManager::GetDefaultConnectionMacAddress()
{
  return m_defaultConnection->GetMacAddress();
}

std::string CNetworkManager::GetDefaultConnectionGateway()
{
  return m_defaultConnection->GetGateway();
}

ConnectionState CNetworkManager::GetDefaultConnectionState()
{
  return m_state;
}

bool CNetworkManager::IsConnected()
{
  return GetDefaultConnectionState() == NETWORK_CONNECTION_STATE_CONNECTED;
}

bool CNetworkManager::IsAvailable(bool wait)
{
  return true;
}

bool CNetworkManager::CanManageConnections()
{
  return m_instance->CanManageConnections();
}

ConnectionList CNetworkManager::GetConnections()
{
  return m_connections;
}

void CNetworkManager::OnConnectionStateChange(ConnectionState state)
{
  ConnectionState oldState = m_state;
  m_state = state;

  if (m_state != oldState)
    CLog::Log(LOGDEBUG, "NetworkManager: State changed %s", ConnectionStateToString(m_state));

  if (oldState != NETWORK_CONNECTION_STATE_CONNECTED && m_state == NETWORK_CONNECTION_STATE_CONNECTED)
    StartServices();
  else if (oldState == NETWORK_CONNECTION_STATE_CONNECTED && oldState != m_state)
    StopServices();
}

void CNetworkManager::OnConnectionChange(CConnectionPtr connection)
{
  if (connection->GetConnectionState() == NETWORK_CONNECTION_STATE_CONNECTED)
    m_defaultConnection = connection;

  CAction action(ACTION_CONNECTIONS_REFRESH );
  g_application.getApplicationMessenger().SendAction(action, WINDOW_DIALOG_ACCESS_POINTS);
}

void CNetworkManager::OnConnectionListChange(ConnectionList list)
{
  m_connections = list;

  for (unsigned int i = 0; i < m_connections.size(); i++)
  {
    if (m_connections[i]->GetConnectionState() == NETWORK_CONNECTION_STATE_CONNECTED)
    {
      m_defaultConnection = m_connections[i];
      OnConnectionStateChange(NETWORK_CONNECTION_STATE_CONNECTED);
      break;
    }
  }

  CAction action(ACTION_CONNECTIONS_REFRESH );
  g_application.getApplicationMessenger().SendAction(action, WINDOW_DIALOG_ACCESS_POINTS);
}

void CNetworkManager::StartServices()
{
  CLog::Log(LOGDEBUG, "NetworkManager: Signaling network services to start");

#ifdef HAS_TIME_SERVER
  g_application.StartTimeServer();
#endif
#ifdef HAS_WEB_SERVER
  g_application.StartWebServer();
#endif
#ifdef HAS_UPNP
  g_application.StartUPnP();
#endif
#ifdef HAS_EVENT_SERVER
  g_application.StartEventServer();
#endif
#ifdef HAS_DBUS_SERVER
  g_application.StartDbusServer();
#endif
#ifdef HAS_JSONRPC
  g_application.StartJSONRPCServer();
#endif
#ifdef HAS_ZEROCONF
  g_application.StartZeroconf();
#endif
  CLastfmScrobbler::GetInstance()->Init();
  CLibrefmScrobbler::GetInstance()->Init();
  g_rssManager.Start();
}

void CNetworkManager::StopServices()
{
  CLog::Log(LOGDEBUG, "NetworkManager: Signaling network services to stop");
  StopServices(false);
  CLog::Log(LOGDEBUG, "NetworkManager: Waiting for network services to stop");
  StopServices(true);
}

void CNetworkManager::StopServices(bool wait)
{
  if (wait)
  {
#ifdef HAS_TIME_SERVER
    g_application.StopTimeServer();
#endif
#ifdef HAS_UPNP
    g_application.StopUPnP(wait);
#endif
#ifdef HAS_ZEROCONF
    g_application.StopZeroconf();
#endif
#ifdef HAS_WEB_SERVER
    g_application.StopWebServer();
#endif    
    CLastfmScrobbler::GetInstance()->Term();
    CLibrefmScrobbler::GetInstance()->Term();
    // smb.Deinit(); if any file is open over samba this will break.

    g_rssManager.Stop();
  }

#ifdef HAS_EVENT_SERVER
  g_application.StopEventServer(wait, false);
#endif
#ifdef HAS_DBUS_SERVER
  g_application.StopDbusServer(wait);
#endif
#ifdef HAS_JSONRPC
    g_application.StopJSONRPCServer(wait);
#endif
}

const char *CNetworkManager::ConnectionStateToString(ConnectionState state)
{
  switch (state)
  {
    case NETWORK_CONNECTION_STATE_FAILURE:
      return "failure";
    case NETWORK_CONNECTION_STATE_DISCONNECTED:
      return "disconnect";
    case NETWORK_CONNECTION_STATE_CONNECTING:
      return "connecting";
    case NETWORK_CONNECTION_STATE_CONNECTED:
      return "connected";
    case NETWORK_CONNECTION_STATE_UNKNOWN:
    default:
      return "unknown";
  }
}
