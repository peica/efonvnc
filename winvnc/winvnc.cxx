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

// -=- VNC Server 4.0 for Windows (WinVNC4)

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>

#include <winvnc/VNCServerWin32.h>
#include <winvnc/AddNewClientDialog.h>

#include <rfb/Logger_stdio.h>
#include <rfb/LogWriter.h>
#include <rfb_wince/AboutDialog.h>
#include <rfb_wince/MsgBox.h>
#include <network/TcpSocket.h>

using namespace winvnc;
using namespace rfb;
using namespace win32;

static LogWriter vlog("main");

TStr rfb::win32::AppName("VNC Server");

static bool runServer = true;
static bool close_console = false;

HINSTANCE gInst;

//
// -=- processParams
//     Read in the command-line parameters and interpret them.
//

static void programInfo() {
  win32::FileVersionInfo inf;
  _tprintf(_T("%s - %s, Version %s\n"),
    inf.getVerString(_T("ProductName")),
    inf.getVerString(_T("FileDescription")),
    inf.getVerString(_T("FileVersion")));
  printf("%s\n", buildTime);
  _tprintf(_T("%s\n\n"), inf.getVerString(_T("LegalCopyright")));
}

static void programUsage() {
	printf("Command-line options:\n");
	printf("  -connect [<host[::port]>]            - Connect an existing VNC server to a listening viewer.\n");
	printf("  -disconnect                          - Disconnect all clients from an existing VNC server.\n");
	printf("  -help                                - Provide usage information.\n");
	printf("  -kill		                           - Kill a running VNC server\n");
	printf("  <setting>=<value>                    - Set the named configuration parameter.\n");
	printf("    (Parameter values specified on the command-line override those specified by other configuration methods.)\n");
	printf("\nLog names:\n");
	LogWriter::listLogWriters();
	printf("\nLog destinations:\n");
	Logger::listLoggers();
	printf("\nAvailable configuration parameters:\n");
	Configuration::listParams();
}

static void MsgBoxOrLog(const char* msg, bool isError=false) {
	if (close_console) {
		MsgBox(0, TStr(msg), (isError ? MB_ICONERROR : MB_ICONINFORMATION) | MB_OK);
	} else {
		if (isError) {
			try {
				vlog.error(msg);
				return;
			} catch (...) {
			}
		}
		fprintf(stderr, "%s\n", msg);
	}
}

//
// -=- main
//
// #if !defined(_CONSOLE)
int APIENTRY _tWinMain(
					 HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
// #else
// int _tmain(int argc, _TCHAR* argv[])
// #endif
{
	gInst = hInstance;
	int result = 0;		

	try {
		// - Initialise the available loggers
// #ifndef NDEBUG
		// _tfreopen(TEXT("\\WinVNC4.txt"),TEXT("ab"),stderr);	  		
		// setvbuf(stderr, 0, 0, 0);

		initStdIOLoggers();

		// initFileLogger("\\WinVNC4.log");
// #endif

// #ifndef NDEBUG
		// - By default, just log errors to stderr
		//logParams.setParam("*:stderr:0");		
		logParams.setParam("*:stderr:120");		
		// - Print program details and process the command line
		programInfo();
// #endif

		TCHAR * connect = _tcsstr(lpCmdLine, _T("-connect "));
		if (connect != NULL) {
			char tmp[256] = {0};
			for (int i=9; i<_tcslen(connect)&&i<256; i++) {
				if (*(connect + i) == ' ') {
					break;
				} else {
					tmp[i-9]=(char)connect[i];
				}
			}
			runServer = false;
			CharArray host;
			host.buf = strDup(tmp);

			if (host.buf) {
				HWND hwnd = FindWindow(0, _T("winvnc::IPC_Interface"));
				if (!hwnd)
					throw rdr::Exception("Unable to locate existing VNC Server.");
				COPYDATASTRUCT copyData;
				copyData.dwData = 1; // *** AddNewClient
				copyData.cbData = strlen(host.buf);
				copyData.lpData = (void*)host.buf;
				printf("Sending connect request to VNC Server...\n");
				if (!SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&copyData))
					MsgBoxOrLog("Connection failed.", true);
			}
		}

		TCHAR * disconnect = _tcsstr(lpCmdLine, _T("-disconnect"));
		if (disconnect != NULL) {
			runServer = false;
			HWND hwnd = FindWindow(0, _T("winvnc::IPC_Interface"));
			if (!hwnd)
				throw rdr::Exception("Unable to locate existing VNC Server.");
			COPYDATASTRUCT copyData;
			copyData.dwData = 2; // *** DisconnectClients
			copyData.lpData = 0;
			copyData.cbData = 0;
			printf("Sending disconnect request to VNC Server...\n");
			if (!SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&copyData))
				MsgBoxOrLog("Failed to disconnect clients.", true);
		}

		TCHAR * kill = _tcsstr(lpCmdLine, _T("-kill"));
		if (kill != NULL) {
			runServer = false;
			HWND hwnd = FindWindow(0, _T("winvnc::IPC_Interface"));
			if (!hwnd)
				throw rdr::Exception("Unable to locate existing VNC Server.");
			COPYDATASTRUCT copyData;
			copyData.dwData = 3; // *** killServer
			copyData.lpData = 0;
			copyData.cbData = 0;
			printf("Sending kill request to VNC Server...\n");
			if (!SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&copyData))
				MsgBoxOrLog("Failed to kill server.", true);
		}

		// - Run the server if required
		if (runServer) {      
			// make sure that only one instance of the vnc server can be started	
			// Thomas 21.05.2008 replace			
			Handle hVNCisRunning = CreateMutex(NULL, FALSE,  L"VNC_is_Running");

			if (GetLastError() == ERROR_ALREADY_EXISTS)
			{		
				return 0;		
			}

			// Start the network subsystem and run the server

			VNCServerWin32 server;
			printf("Starting User-Mode VNC Server.\n"); 	  

			result = server.run();
		}

		vlog.debug("WinVNC service destroyed");
	} catch (rdr::Exception& e) {
		MsgBoxOrLog(e.str(), true);
	}

	vlog.debug("WinVNC process quitting");
	return result;
}
