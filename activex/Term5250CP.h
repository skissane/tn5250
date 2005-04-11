/* 
 * Copyright (C) 2003 Marc-Antoine Ruel at Cardinal Health
 * 
 * This file is part of TN5250
 *
 * TN5250 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 * 
 * TN5250 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 *
 */

#pragma once

#pragma warning(push)
#pragma warning(disable:4946) // reinterpret_cast used between related classes: '' and ''

template <class T>
class CProxy_ITerm5250Events : public ATL::IConnectionPointImpl<T, &__uuidof(_ITerm5250Events), ATL::CComDynamicUnkArray>
{
    //Warning this class may be recreated by the wizard.
public:
    virtual ~CProxy_ITerm5250Events() {
    }

    VOID Fire_Connected()
    {
        FUNC_ENTER1(" %d", m_vec.GetSize());
        Fire_Anything(0x1);
    }

    VOID Fire_Disconnected()
    {
        FUNC_ENTER1(" %d", m_vec.GetSize());
        Fire_Anything(0x2);
    }

protected:
    void Fire_Anything(int Index)
    {
        T * const pT = static_cast<T*>(this);
        const int nConnections = m_vec.GetSize();
        //CComVariant pvars[2];

        for (int nConnectionIndex = 0; nConnectionIndex < nConnections; ++nConnectionIndex)
        {
            pT->Lock();
            ATL::CComPtr<IDispatch> pDispatch(reinterpret_cast<IDispatch*>(m_vec.GetAt(nConnectionIndex)));
            pT->Unlock();
            if (pDispatch != NULL)
            {
                //pvars[1].vt = VT_I4;
                //pvars[1].lVal= x;
                //pvars[0].vt = VT_I4;
                //pvars[0].lVal= y;
                //DISPPARAMS disp = { pvars, NULL, 2, 0 };
                DISPPARAMS disp = { NULL, NULL, 0, 0 };
                pDispatch->Invoke(Index, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
            }
        }
    }
};
#pragma warning(pop)
