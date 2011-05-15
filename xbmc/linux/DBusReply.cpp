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

#include "DBusReply.h"
#ifdef HAS_DBUS
#include "utils/log.h"
#include <dbus/dbus.h>

using namespace std;

CDBusReply::CDBusReply(DBusMessage *message)
{
  m_message = message;
  m_iterator = NULL;
  m_hasMoreArguments = true;
  m_error = false;

  if (m_message)
  {
    m_iterator = new DBusMessageIter();
    dbus_message_iter_init (m_message, m_iterator);
    dbus_message_ref(m_message);
  }
}

CDBusReply::CDBusReply()
{
  m_message = NULL;
  m_iterator = NULL;
  m_hasMoreArguments = false;
  m_error = true;
}

CDBusReply::~CDBusReply()
{
  delete m_iterator;
  m_iterator = NULL;

  if (m_message)
    dbus_message_unref(m_message);
}

CVariant CDBusReply::GetNextArgument()
{
  CVariant argument;

  if (m_iterator && m_hasMoreArguments)
  {
    ParseIter(m_iterator, argument);
    m_hasMoreArguments = dbus_message_iter_next(m_iterator);
  }

  return argument;
}

bool CDBusReply::IsErrorSet()
{
  return m_error;
}

string CDBusReply::ParseIter(DBusMessageIter *iter, CVariant &parsed)
{
  int type = dbus_message_iter_get_arg_type (iter);
  string key;

  if (type == DBUS_TYPE_INVALID)
    return key;
    
  switch (type)
  {
    case DBUS_TYPE_OBJECT_PATH:
    case DBUS_TYPE_STRING:
    {
      char *val;
      dbus_message_iter_get_basic (iter, &val);

      parsed = val;
      break;
    }

    case DBUS_TYPE_BOOLEAN:
    {
      dbus_bool_t val;
      dbus_message_iter_get_basic (iter, &val);
      parsed = (bool)val;
      break;
    }

    case DBUS_TYPE_INT16:
    {
      dbus_int16_t val;
      dbus_message_iter_get_basic (iter, &val);
      parsed = (int64_t)val;
      break;
    }

    case DBUS_TYPE_INT32:
    {
      dbus_int32_t val;
      dbus_message_iter_get_basic (iter, &val);
      parsed = (int64_t)val;
      break;
    }

    case DBUS_TYPE_INT64:
    {
      dbus_int64_t val;
      dbus_message_iter_get_basic (iter, &val);
      parsed = (int64_t)val;
      break;
    }

    case DBUS_TYPE_UINT16:
    {
      dbus_uint16_t val;
      dbus_message_iter_get_basic (iter, &val);
      parsed = (uint64_t)val;
      break;
    }

    case DBUS_TYPE_UINT32:
    {
      dbus_uint32_t val;
      dbus_message_iter_get_basic (iter, &val);
      parsed = (uint64_t)val;
      break;
    }

    case DBUS_TYPE_UINT64:
    {
      dbus_uint64_t val;
      dbus_message_iter_get_basic (iter, &val);
      parsed = (uint64_t)val;
      break;
    }

    case DBUS_TYPE_BYTE:
    {
      // TODO variant should have byte support since it could be used for useful stuff.
      unsigned char val;
      dbus_message_iter_get_basic (iter, &val);
      parsed = (int64_t)val;
      break;
    }

    case DBUS_TYPE_VARIANT:
    {
      DBusMessageIter variant;
      dbus_message_iter_recurse (iter, &variant);

      ParseIter(&variant, parsed);
      break;
    }

    case DBUS_TYPE_DICT_ENTRY:
    {
      DBusMessageIter dictIter;

      dbus_message_iter_recurse (iter, &dictIter);

      CVariant keyVariant;
      ParseIter(&dictIter, keyVariant);
      key = keyVariant.asString();

      dbus_message_iter_next (&dictIter);
      ParseIter(&dictIter, parsed);
      break;
    }

    case DBUS_TYPE_STRUCT:
    case DBUS_TYPE_ARRAY:
    {
      DBusMessageIter containerIter;

      dbus_message_iter_recurse (iter, &containerIter);

      do
      {
        CVariant value;

        string key = ParseIter(&containerIter, value);
        if (key.size() == 0)
          parsed.push_back(value);
        else if (!value.isNull())
          parsed[key] = value;
        else
          CLog::Log(LOGWARNING, "DBus: recieved a key '%s' with no value. Don't add to map", key.c_str());

      } while (dbus_message_iter_next(&containerIter));
      break;
    }

    default:
      CLog::Log(LOGWARNING, "DBus: unkown argument type '%c'", type);
      break;
  }

  return key;
}
#endif
