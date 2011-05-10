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

#include "NullNetworkManager.h"

bool CNullConnection::Connect(IPassphraseStorage *storage, const CIPConfig &ipconfig)
{
  return false;
}

ConnectionState CNullConnection::GetConnectionState() const
{
  return NETWORK_CONNECTION_STATE_CONNECTED;
}

std::string CNullConnection::GetName() const
{
  return "Unkown connection";
}

std::string CNullConnection::GetIP() const
{
  return "127.0.0.1";
}

std::string CNullConnection::GetNetmask() const              { return "255.255.255.0"; }
std::string CNullConnection::GetMacAddress() const           { return "00:00:00:00:00:00"; }
std::string CNullConnection::GetGateway() const              { return ""; }

unsigned int CNullConnection::GetStrength() const            { return 100; }

EncryptionType CNullConnection::GetEncryption() const        { return NETWORK_CONNECTION_ENCRYPTION_NONE; }
unsigned int CNullConnection::GetConnectionSpeed() const     { return 100; }
ConnectionType CNullConnection::GetConnectionType() const    { return NETWORK_CONNECTION_TYPE_WIRED; }
};

class CNullNetworkManager : public INetworkManager
{
CNullNetworkManager::~CNullNetworkManager() { }

bool CNullNetworkManager::CanManageConnections() { return false; }

ConnectionList CNullNetworkManager::GetConnections() { ConnectionList list; list.push_back(CConnectionPtr(new CNullConnection())); return list; }

bool CNullNetworkManager::PumpNetworkEvents(INetworkEventsCallback *callback) { return true; }
};
