/* 
 * Copyright (C) 2004 Marc-Antoine Ruel at Cardinal Health
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
#define MAX_HOST_SIZE 255
#define MAX_DEVICE_SIZE 10

class CDlgConnect : public ATL::CSimpleDialog<IDD_DIALOG_CONNECT>
{
public:
    CString m_IP;
    CString m_Device;
    long    m_CodePage;
    bool    m_Wide;

protected:
    BEGIN_MSG_MAP(thisClass)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnOK)
        CHAIN_MSG_MAP(super)
    END_MSG_MAP()

    LRESULT OnInitDialog(UINT, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        SendDlgItemMessage(IDC_EDIT_CODE_PAGE,EM_LIMITTEXT, (WPARAM)9, 0);
        SendDlgItemMessage(IDC_EDIT_DEVICE, EM_LIMITTEXT, (WPARAM)MAX_DEVICE_SIZE, 0);
        SendDlgItemMessage(IDC_EDIT_IP, EM_LIMITTEXT, (WPARAM)MAX_HOST_SIZE, 0);
        if ( !m_IP.IsEmpty() )
            SetDlgItemText(IDC_EDIT_IP, m_IP);
        if ( !m_Device.IsEmpty() )
            SetDlgItemText(IDC_EDIT_DEVICE, m_Device);
        SetDlgItemInt(IDC_EDIT_CODE_PAGE, m_CodePage);
        CheckRadioButton(IDC_RADIO_24, IDC_RADIO_27, m_Wide?IDC_RADIO_27:IDC_RADIO_24);
        SendDlgItemMessage(IDC_EDIT_IP, EM_SETSEL, 0, -1);
        GetDlgItem(IDC_EDIT_IP).SetFocus();
        return 0;
    }

    LRESULT OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
    {
        GetDlgItemText(IDC_EDIT_IP, m_IP);
        GetDlgItemText(IDC_EDIT_DEVICE, m_Device);
        m_CodePage = GetDlgItemInt(IDC_EDIT_CODE_PAGE, NULL, FALSE);
        m_Wide = (IsDlgButtonChecked(IDC_RADIO_27)==BST_CHECKED);
        bHandled = FALSE;
        return 0;
    }
};
