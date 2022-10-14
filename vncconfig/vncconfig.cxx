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

#include <windows.h>
#include <commctrl.h>
#include <string.h>

#include "resource.h"
#include <rfb/Logger_stdio.h>
#include <rfb/LogWriter.h>
#include <rfb/SSecurityFactoryStandard.h>
#include <rfb_wince/Dialog.h>
#include <rfb_wince/RegConfig.h>

using namespace rfb;
using namespace rfb::win32;

static LogWriter vlog("main");


#include <vncconfig/Authentication.h>
#include <vncconfig/Connections.h>
#include <vncconfig/Sharing.h>

TStr rfb::win32::AppName("VNC Config");

#ifdef _DEBUG
BoolParameter captureDialogs("CaptureDialogs", "", false);
#endif

HKEY configKey = HKEY_CURRENT_USER;

void
processParams(int argc, char* argv[]) {
  for (int i=1; i<argc; i++) {
    if (_stricmp(argv[i], "-service") == 0) {
      configKey = HKEY_LOCAL_MACHINE;
    } else if (_stricmp(argv[i], "-user") == 0) {
      configKey = HKEY_CURRENT_USER;
    } else {
      // Try to process <option>=<value>, or -<bool>
      if (Configuration::setParam(argv[i], true))
        continue;
      // Try to process -<option> <value>
      if ((argv[i][0] == '-') && (i+1 < argc)) {
        if (Configuration::setParam(&argv[i][1], argv[i+1], true)) {
          i++;
          continue;
        }
      }
    }
  }
}


int APIENTRY _tWinMain(HINSTANCE inst, HINSTANCE prev, LPTSTR cmdLine, int cmdShow) {

// Thomas 21.05.2008 remove debug output		
  // Configure debugging output
//#ifdef _DEBUG
//  AllocConsole();
//  freopen("CONIN$","rb",stdin);
//  freopen("CONOUT$","wb",stdout);
//  freopen("CONOUT$","wb",stderr);
//  setbuf(stderr, 0);
//  initStdIOLoggers();
//  LogWriter vlog("main");
//  logParams.setParam("*:stderr:100");
//  vlog.info("Starting vncconfig applet");
//#endif

  try {
    try {
      // Process command-line args
#if 0
      int argc = __argc;
      char** argv = __argv;
      processParams(argc, argv);
#endif

	  // Override whatever security it already had (NT only)
      bool warnOnChangePassword = false;

      // Create the required configuration registry key
      RegKey rootKey;
      rootKey.createKey(configKey, _T("Software\\RealVNC\\WinVNC4"));
  
      // Start a RegConfig thread, to load in existing settings
      RegConfigThread config;
      config.start(configKey, _T("Software\\RealVNC\\WinVNC4"));

      // Build the dialog
      std::list<PropSheetPage*> pages;
      AuthenticationPage auth(rootKey); pages.push_back(&auth);
      auth.setWarnPasswdInsecure(warnOnChangePassword);
      ConnectionsPage conn(rootKey); pages.push_back(&conn);
      SharingPage sharing(rootKey); pages.push_back(&sharing);

      // Load the default icon to use
      //HICON icon = (HICON)LoadImage(inst, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 0, 0, LR_SHARED);
      HICON icon = (HICON)LoadImage(inst, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);

      // Create the PropertySheet handler
      TCHAR* propSheetTitle = _T("VNC Server Properties (Service-Mode)");
      if (configKey == HKEY_CURRENT_USER)
        propSheetTitle = _T("VNC Server Properties (User-Mode)");
      PropSheet sheet(inst, propSheetTitle, pages, icon);

#ifdef _DEBUG
      vlog.debug("capture dialogs=%s", captureDialogs ? "true" : "false");
      sheet.showPropSheet(0, true, false, captureDialogs);
#else
      sheet.showPropSheet(0, true, false);
#endif

	  DestroyIcon(icon);
	  icon = 0;
    } catch (rdr::SystemException& e) {
      switch (e.err) {
      case ERROR_ACCESS_DENIED:
        MsgBox(0, _T("You do not have sufficient access rights to run the VNC Configuration applet"),
               MB_ICONSTOP | MB_OK);
        return 1;
      };
      throw;
    }

  } catch (rdr::Exception& e) {
    MsgBox(NULL, TStr(e.str()), MB_ICONEXCLAMATION | MB_OK);
    return 1;
  }
  
  return 0;
}
