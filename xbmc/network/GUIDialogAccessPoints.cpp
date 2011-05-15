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

#include "GUIDialogAccessPoints.h"
#include "dialogs/GUIDialogKeyboard.h"
#include "Application.h"
#include "FileItem.h"
#include "guilib/LocalizeStrings.h"
#include "utils/JobManager.h"
#include "ConnectionJob.h"

#define CONTROL_ACCESS_POINTS 3

CGUIDialogAccessPoints::CGUIDialogAccessPoints(void)
    : CGUIDialog(WINDOW_DIALOG_ACCESS_POINTS, "DialogAccessPoints.xml")
{
  m_connectionsFileList = new CFileItemList;
}

CGUIDialogAccessPoints::~CGUIDialogAccessPoints(void)
{
  delete m_connectionsFileList;
}

bool CGUIDialogAccessPoints::OnAction(const CAction &action)
{
  if (action.GetID() == ACTION_SELECT_ITEM)
  {
    CGUIMessage msg(GUI_MSG_ITEM_SELECTED, GetID(), CONTROL_ACCESS_POINTS);
    OnMessage(msg);
    int iItem = msg.GetParam1();

    ConnectionList connections = g_application.getNetworkManager().GetConnections();
    CJobManager::GetInstance().AddJob(new CConnectionJob(connections[iItem], &g_application.getKeyringManager()), this);

    return true;
  }
  else if (action.GetID() == 300)
  {
    UpdateConnectionList();
    return true;
  }

  return CGUIDialog::OnAction(action);
}

void CGUIDialogAccessPoints::OnInitWindow()
{
  CGUIDialog::OnInitWindow();

  UpdateConnectionList();
}

void CGUIDialogAccessPoints::OnJobComplete(unsigned int jobID, bool success, CJob *job)
{
  if (success)
    Close();
}

void CGUIDialogAccessPoints::UpdateConnectionList()
{
  m_connectionsFileList->Clear();

  CGUIMessage msgReset(GUI_MSG_LABEL_RESET, GetID(), CONTROL_ACCESS_POINTS);
  OnMessage(msgReset);

  ConnectionList connections = g_application.getNetworkManager().GetConnections();

  for (int i = 0; i < (int) connections.size(); i++)
  {
    CFileItemPtr item(new CFileItem(connections[i]->GetName()));

    if (connections[i]->GetConnectionType() == NETWORK_CONNECTION_TYPE_WIFI)
    {
      item->SetProperty("signal", (int)(connections[i]->GetStrength() / 20));
      item->SetProperty("encryption", EncryptionToString(connections[i]->GetEncryption()));
    }

    item->SetProperty("type", ConnectionTypeToString(connections[i]->GetConnectionType()));
    item->SetProperty("state", ConnectionStateToString(connections[i]->GetConnectionState()));
 
    m_connectionsFileList->Add(item);
  }

  CGUIMessage msg(GUI_MSG_LABEL_BIND, GetID(), CONTROL_ACCESS_POINTS, 0, 0, m_connectionsFileList);
  OnMessage(msg);
}

const char *CGUIDialogAccessPoints::ConnectionStateToString(ConnectionState state)
{
  switch (state)
  {
    case NETWORK_CONNECTION_STATE_DISCONNECTED:
      return "disconnected";
    case NETWORK_CONNECTION_STATE_CONNECTING:
      return "connecting";
    case NETWORK_CONNECTION_STATE_CONNECTED:
      return "connected";
    case NETWORK_CONNECTION_STATE_FAILURE:
      return "failure";
    case NETWORK_CONNECTION_STATE_UNKNOWN:
    default:
      return "unknown";
  }

  return "";
}

const char *CGUIDialogAccessPoints::ConnectionTypeToString(ConnectionType type)
{
  switch (type)
  {
    case NETWORK_CONNECTION_TYPE_WIRED:
      return "wired";
    case NETWORK_CONNECTION_TYPE_WIFI:
      return "wifi";
    case NETWORK_CONNECTION_TYPE_UNKNOWN:
    default:
      return "unknown";
  }

  return "";
}

const char *CGUIDialogAccessPoints::EncryptionToString(EncryptionType type)
{
  switch (type)
  {
    case NETWORK_CONNECTION_ENCRYPTION_NONE:
      return "";
    case NETWORK_CONNECTION_ENCRYPTION_WEP:
      return "wep";
    case NETWORK_CONNECTION_ENCRYPTION_WPA:
      return "wpa";
    case NETWORK_CONNECTION_ENCRYPTION_WPA2:
      return "wpa2";
    case NETWORK_CONNECTION_ENCRYPTION_IEEE8021x:
      return "wpa-rsn";
    case NETWORK_CONNECTION_ENCRYPTION_UNKNOWN:
    default:
      return "unknown";
  }

  return "";
}
