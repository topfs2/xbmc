#pragma once
/*
 *      Copyright (C) 2011 Tobias Arrskog
 *      https://github.com/topfs2/Service
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

template<class S> class CServiceProxy
{
private:
  S *m_ptr;
public:
  CServiceProxy() : m_ptr(0)
  {
    m_ptr = GetInstance();
    if (m_ptr == NULL)
      throw;
  }

  S *operator-> ()
  {
    return m_ptr;
  }

  /* Any service who whishes to work needs to define this function for their particular service. They can do so by using the following template in their cpp, the compiler will choose that one over this default:

    template<>
    inline
    CFooService *CServiceProxy<CFooService>::GetInstance()
    {
      ...
    }
  */
  static S *GetInstance() { return NULL; }
};
