/* Copyright (C) 2002-2005 RealVNC Ltd.  All Rights Reserved.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */

// -=- CView.h

// An instance of the CView class is created for each VNC Viewer connection.

#ifndef __RFB_WIN32_TRAY_ICON_H__
#define __RFB_WIN32_TRAY_ICON_H__

#include <windows.h>
#include <shellapi.h>
#include <rfb_wince/MsgWindow.h>
#include <rdr/Exception.h>

extern HINSTANCE gInst;

namespace rfb {

  namespace win32 {

    class TrayIcon : public MsgWindow {
    public:
      TrayIcon() : MsgWindow(_T("VNCTray")) {
#ifdef NOTIFYICONDATA_V1_SIZE
        nid.cbSize = NOTIFYICONDATA_V1_SIZE;
#else
        nid.cbSize = sizeof(NOTIFYICONDATA);
#endif

        nid.hWnd = getHandle();
		
		// Thomas 21.05.2008 change 
        //nid.uID = 0;
        nid.uID = 100; // Per WinCE SDK docs, values from 0 to 12 are reserved and should not be used.

        nid.hIcon = 0;
        nid.uFlags = NIF_ICON | NIF_MESSAGE;
        nid.uCallbackMessage = WM_USER;
      }
      virtual ~TrayIcon() {
        remove();
		if (nid.hIcon != 0) {
			DestroyIcon(nid.hIcon);
			nid.hIcon = 0;
		}
      }
      bool setIcon(UINT icon) {
        if (icon == 0) {
          return remove();
        } else {       	
			// Notice: WinCE don't support the LR_SHARED, so 
			// we have to release the GDI object which allocate
			// at last setIcon call
			if (nid.hIcon != 0) {
				DestroyIcon(nid.hIcon);
				nid.hIcon = 0;
			}
	       nid.hIcon = (HICON)LoadImage(gInst, MAKEINTRESOURCE(icon), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		   return refresh();
        }
      }
      bool setToolTip(const TCHAR* text) {
        if (text == 0) {
          nid.uFlags &= ~NIF_TIP;
        } else {
          const int tipLen = sizeof(nid.szTip)/sizeof(TCHAR);
          _tcsncpy(nid.szTip, text, tipLen);
          nid.szTip[tipLen-1] = 0;
          nid.uFlags |= NIF_TIP;
        }
        return refresh();
      }
      bool remove() {
        return Shell_NotifyIcon(NIM_DELETE, &nid) != 0;
      }
      bool refresh() {
        return Shell_NotifyIcon(NIM_MODIFY, &nid) || Shell_NotifyIcon(NIM_ADD, &nid);
      }
    protected:
      NOTIFYICONDATA nid;
    };

  };

};

#endif


