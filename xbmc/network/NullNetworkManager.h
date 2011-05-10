#pragma once
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

#include "INetworkManager.h"

class CNullConnection : public IConnection
{
public:
  virtual ~CNullConnection() { }

  virtual bool Connect(IPassphraseStorage *storage, const CIPConfig &ipconfig) { return false; }
  virtual ConnectionState GetConnectionState() const  { return NETWORK_CONNECTION_STATE_CONNECTED; }

  virtual std::string GetName() const                 { return "Unkown connection"; }

  virtual std::string GetIP() const                   { return "127.0.0.1"; }
  virtual std::string GetNetmask() const              { return "255.255.255.0"; }
  virtual std::string GetMacAddress() const           { return "00:00:00:00:00:00"; }
  virtual std::string GetGateway() const              { return ""; }

  virtual unsigned int GetStrength() const            { return 100; }

  virtual EncryptionType GetEncryption() const        { return NETWORK_CONNECTION_ENCRYPTION_NONE; }
  virtual unsigned int GetConnectionSpeed() const     { return 100; }
  virtual ConnectionType GetConnectionType() const    { return NETWORK_CONNECTION_TYPE_WIRED; }
};

class CNullNetworkManager : public INetworkManager
{
  virtual ~CNullNetworkManager() { }

  virtual bool CanManageConnections() { return false; }

  virtual ConnectionList GetConnections() { ConnectionList list; list.push_back(CConnectionPtr(new CNullConnection())); return list; }

  virtual bool PumpNetworkEvents(INetworkEventsCallback *callback) { return true; }
};
