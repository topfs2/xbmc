/*
 *      Copyright (C) 2005-2009 Team XBMC
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
#include "DBusUtil.h"
#ifdef HAS_DBUS
#include "utils/log.h"

CVariant CDBusUtil::GetVariant(const char *destination, const char *object, const char *interface, const char *property)
{
//dbus-send --system --print-reply --dest=destination object org.freedesktop.DBus.Properties.Get string:interface string:property
  CDBusMessage message(destination, object, DBUS_INTERFACE_PROPERTIES, "Get");
  CVariant result;

  if (message.AppendArgument(interface) && message.AppendArgument(property))
  {
    CDBusReplyPtr reply = message.SendSystem();
    result = reply->GetNextArgument();
  }
  else
    CLog::Log(LOGERROR, "DBus: append arguments failed");

  return result;
}

CVariant CDBusUtil::GetAll(const char *destination, const char *object, const char *interface)
{
  CDBusMessage message(destination, object, DBUS_INTERFACE_PROPERTIES, "GetAll");
  CVariant properties;
  message.AppendArgument(interface);

  CDBusReplyPtr reply = message.SendSystem();
  properties = reply->GetNextArgument();

  return properties;
}
#endif
