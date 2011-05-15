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

#include "system.h"
#ifdef HAS_DBUS
#include "network/IConnection.h"
#include "linux/DBusUtil.h"
#include <dbus/dbus.h>

class CConnmanConnection : public IConnection
{
public:
  CConnmanConnection(const char *serviceObject);
  virtual ~CConnmanConnection();

  virtual bool Connect(IPassphraseStorage *storage, const CIPConfig &ipconfig);
  virtual ConnectionState GetConnectionState() const;

  virtual std::string GetName() const;

  virtual std::string GetIP() const;
  virtual std::string GetNetmask() const;
  virtual std::string GetMacAddress() const;
  virtual std::string GetGateway() const;

  virtual unsigned int GetStrength() const;
  virtual EncryptionType GetEncryption() const;
  virtual unsigned int GetConnectionSpeed() const;

  virtual ConnectionType GetConnectionType() const;

  bool PumpNetworkEvents();

  static ConnectionState ParseConnectionState(const char *stateString);
private:
  void UpdateConnection();

  CVariant m_properties;

  std::string m_name;
  std::string m_IP;
  std::string m_netmask;
  std::string m_macaddress;
  std::string m_gateway;

  std::string m_serviceObject;

  unsigned int m_strength;
  unsigned int m_speed;

  ConnectionState m_state;
  EncryptionType m_encryption;
  ConnectionType m_type;

  DBusConnection *m_connection;
  DBusError m_error;
};
#endif
