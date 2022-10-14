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

#include <rfb_wince/MonitorInfo.h>
#include <rfb_wince/Win32Util.h>
#include <rdr/Exception.h>
#include <rfb/LogWriter.h>

using namespace rfb;
using namespace win32;

static LogWriter vlog("MonitorInfo");


MonitorInfo::MonitorInfo(HWND window) {
  cbSize = sizeof(MonitorInfo);
  szDevice[0] = 0;

  // Legacy fallbacks - just return the desktop settings
  vlog.debug("using legacy fall-backs");
  HWND desktop = GetDesktopWindow();
  GetWindowRect(desktop, &rcMonitor);
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
  dwFlags = 0;
}

MonitorInfo::MonitorInfo(const RECT& r) {
  cbSize = sizeof(MonitorInfo);
  szDevice[0] = 0;

  // Legacy fallbacks - just return the desktop settings
  vlog.debug("using legacy fall-backs");
  HWND desktop = GetDesktopWindow();
  GetWindowRect(desktop, &rcMonitor);
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
  dwFlags = 0;
}

MonitorInfo::MonitorInfo(const char* devName) {
  // If multi-monitor is not built, or not supported by the OS,
  //   or if the named monitor is not found, revert to the primary monitor.
  vlog.debug("reverting to primary monitor");
  cbSize = sizeof(MonitorInfo);
  szDevice[0] = 0;

  HWND desktop = GetDesktopWindow();
  GetWindowRect(desktop, &rcMonitor);
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
  dwFlags = 0;
}

void MonitorInfo::moveTo(HWND handle) {
  vlog.debug("moveTo monitor=%s", szDevice);
}

void MonitorInfo::clipTo(RECT* r) {
  vlog.debug("clipTo monitor=%s", szDevice);

  if (r->top < rcWork.top) {
    r->bottom += rcWork.top - r->top; r->top = rcWork.top;
  }
  if (r->left < rcWork.left) {
    r->right += rcWork.left - r->left; r->left = rcWork.left;
  }
  if (r->bottom > rcWork.bottom) {
    r->top += rcWork.bottom - r->bottom; r->bottom = rcWork.bottom;
  }
  if (r->right > rcWork.right) {
    r->left += rcWork.right - r->right; r->right = rcWork.right;
  }
  r->left = max(r->left, rcWork.left);
  r->right = min(r->right, rcWork.right);
  r->top = max(r->top, rcWork.top);
  r->bottom = min(r->bottom, rcWork.bottom);
}

void MonitorInfo::clipTo(HWND handle) {
  RECT r;
  GetWindowRect(handle, &r);
  clipTo(&r);
  SetWindowPos(handle, 0, r.left, r.top, r.right-r.left, r.bottom-r.top,
               SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}


