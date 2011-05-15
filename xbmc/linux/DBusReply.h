#pragma once
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

#include "system.h"
#ifdef HAS_DBUS
#include "utils/Variant.h"
#include <boost/shared_ptr.hpp>

struct DBusMessage;
struct DBusMessageIter;

class CDBusReply : public CVariant
{
public:
  CDBusReply(DBusMessage *message);
  CDBusReply();
  virtual ~CDBusReply();

  CVariant GetNextArgument();
  bool IsErrorSet();
private:
  std::string ParseIter(DBusMessageIter *iter, CVariant &parsed);

  DBusMessageIter *m_iterator;
  DBusMessage *m_message;
  bool m_hasMoreArguments;
  bool m_error;
};

typedef boost::shared_ptr<CDBusReply> CDBusReplyPtr;
#endif
