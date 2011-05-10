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
#include <string>
#include <vector>
#include "IPassphraseStorage.h"
#include <boost/shared_ptr.hpp>

enum ConnectionType
{
  NETWORK_CONNECTION_TYPE_UNKNOWN = 0,
  NETWORK_CONNECTION_TYPE_WIRED,
  NETWORK_CONNECTION_TYPE_WIFI
};

enum ConnectionState
{
  NETWORK_CONNECTION_STATE_UNKNOWN = 0,
  NETWORK_CONNECTION_STATE_FAILURE,
  NETWORK_CONNECTION_STATE_DISCONNECTED,
  NETWORK_CONNECTION_STATE_CONNECTING,
  NETWORK_CONNECTION_STATE_CONNECTED
};

enum EncryptionType
{
  NETWORK_CONNECTION_ENCRYPTION_UNKNOWN = 0, // This should be used to flag accesspoints which have some encryption which we cannot connect to.
  NETWORK_CONNECTION_ENCRYPTION_NONE,
  NETWORK_CONNECTION_ENCRYPTION_WEP,
  NETWORK_CONNECTION_ENCRYPTION_WPA,
  NETWORK_CONNECTION_ENCRYPTION_WPA2,
  NETWORK_CONNECTION_ENCRYPTION_IEEE8021x
};

enum IPConfigMethod
{
  IP_CONFIG_STATIC,
  IP_CONFIG_DHCP
};

class CIPConfig
{
public:
  CIPConfig()
  {
    reset();
  }

  CIPConfig(IPConfigMethod method, const std::string &IP, const std::string &netmask)
  {
    m_method = method;
    m_IP = IP;
    m_netmask = netmask;
  }

  void reset()
  {
    m_method = IP_CONFIG_DHCP;
    m_IP = "";
    m_netmask = "";
  }

  IPConfigMethod m_method;
  std::string m_IP;
  std::string m_netmask;
};

class IConnection
{
public:
  virtual ~IConnection() { }

  /*!
   \brief Connect to connection

   \param storage a passphrase provider
   \param ipconfig a configuration for how to acquire IP
   \returns true if connected, false if not.
   \sa IPassphraseStorage CIPConfig
   */
  virtual bool Connect(IPassphraseStorage *storage, const CIPConfig &ipconfig) = 0;

  /*!
   \brief Get the state of the connection

   \return The state the connection is currently in.
   \sa ConnectionState
   */
  virtual ConnectionState GetConnectionState() const = 0;

  /*!
   \brief Get the name of the connection

   \return The name of the connection
   \sa IConnection
   */
  virtual std::string GetName() const = 0;

  /*!
   \brief Get the IP of the connection

   \return The IP of the connection
   \sa IConnection
   */
  virtual std::string GetIP() const = 0;

  /*!
   \brief Get the netmask of the connection

   \return The netmask of the connection
   \sa IConnection
   */
  virtual std::string GetNetmask() const = 0;

  /*!
   \brief Get the mac address of the connection

   \return The mac address of the connection
   \sa IConnection
   */
  virtual std::string GetMacAddress() const = 0;

  /*!
   \brief Get the gateway address of the connection

   \return The gateway address of the connection
   \sa IConnection
   */
  virtual std::string GetGateway() const = 0;

  /*!
   \brief The signal strength of the connection

   \return The signal strength of the connection
   \sa IConnection
   */
  virtual unsigned int GetStrength() const = 0;

  /*!
   \brief Get the encryption used by the connection

   \return The encryption used by the connection
   \sa EncryptionType
   */
  virtual EncryptionType GetEncryption() const = 0;

  /*!
   \brief Get the speed of the connection

   \return The speed of the connection
   \sa IConnection
   */
  virtual unsigned int GetConnectionSpeed() const = 0;

  /*!
   \brief Get the connection type

   \return The connection type
   \sa ConnectionType
   */
  virtual ConnectionType GetConnectionType() const = 0;
};

typedef boost::shared_ptr<IConnection> CConnectionPtr;
typedef std::vector<CConnectionPtr> ConnectionList;
