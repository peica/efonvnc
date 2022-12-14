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

// -=- SInput.cxx
//
// A number of routines that accept VNC input event data and perform
// the appropriate actions under Win32

#define XK_MISCELLANY
#define XK_LATIN1
#define XK_CURRENCY
#include <rfb/keysymdef.h>

#include <tchar.h>
#include <rfb_wince/SInput.h>
#include <rfb_wince/OSVersion.h>
#include <rfb_wince/keymap.h>
#include <rdr/Exception.h>
#include <rfb/LogWriter.h>

using namespace rfb;

static LogWriter vlog("SInput");

//
// -=- Pointer implementation for Win32
//

static DWORD buttonDownMapping[8] = {
  MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_RIGHTDOWN,
  MOUSEEVENTF_WHEEL, MOUSEEVENTF_WHEEL, 0, 0, 0
};

static DWORD buttonUpMapping[8] = {
  MOUSEEVENTF_LEFTUP, MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_RIGHTUP,
  MOUSEEVENTF_WHEEL, MOUSEEVENTF_WHEEL, 0, 0, 0
};

static DWORD buttonDataMapping[8] = {
  0, 0, 0, 120, -120, 0, 0, 0
};

win32::SPointer::SPointer()
  : last_buttonmask(0)
{
}

void
win32::SPointer::pointerEvent(const Point& pos, int buttonmask)
{
  // - We are specifying absolute coordinates
  DWORD flags = MOUSEEVENTF_ABSOLUTE;

  // - Has the pointer moved since the last event?
  if (!last_position.equals(pos))
    flags |= MOUSEEVENTF_MOVE;

  DWORD data = 0;
  for (int i = 0; i < 8; i++) {
    if ((buttonmask & (1<<i)) != (last_buttonmask & (1<<i))) {
      if (buttonmask & (1<<i)) {
        flags |= buttonDownMapping[i];
        if (buttonDataMapping[i]) {
          if (data) vlog.info("warning - two buttons set mouse_event data field");
          data = buttonDataMapping[i];
        }
      } else {
        flags |= buttonUpMapping[i];
      }
    }
  }

  last_position = pos;
  last_buttonmask = buttonmask;

  Rect primaryDisplay(0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
  if (primaryDisplay.contains(pos)) {
    // mouse_event wants coordinates specified as a proportion of the
    // primary display's size, scaled to the range 0 to 65535
    Point scaled;
    scaled.x = (pos.x * 65535) / (primaryDisplay.width()-1);
    scaled.y = (pos.y * 65535) / (primaryDisplay.height()-1);
    ::mouse_event(flags, scaled.x, scaled.y, data, 0);
  } else {
    // The event lies outside the primary monitor.  Under Win2K, we can just use
    // SendInput, which allows us to provide coordinates scaled to the virtual desktop.
    // SendInput is available on all multi-monitor-aware platforms.
  }
}

//
// -=- Keyboard implementation
//

BoolParameter rfb::win32::SKeyboard::deadKeyAware("DeadKeyAware",
  "Whether to assume the viewer has already interpreted dead key sequences "
  "into latin-1 characters", true);

static bool oneShift;

// The keysymToAscii table transforms a couple of awkward keysyms into their
// ASCII equivalents.
struct  keysymToAscii_t {
  rdr::U32 keysym;
  rdr::U8 ascii;
};

keysymToAscii_t keysymToAscii[] = {
  { XK_KP_Space, ' ' },
  { XK_KP_Equal, '=' },
};

rdr::U8 latin1DeadChars[] = {
  XK_grave, XK_acute, XK_asciicircum, XK_diaeresis, XK_degree, XK_cedilla,
  XK_asciitilde
};

struct latin1ToDeadChars_t {
  rdr::U8 latin1Char;
  rdr::U8 deadChar;
  rdr::U8 baseChar;
};

latin1ToDeadChars_t latin1ToDeadChars[] = {

  { XK_Agrave, XK_grave, XK_A },
  { XK_Egrave, XK_grave, XK_E },
  { XK_Igrave, XK_grave, XK_I },
  { XK_Ograve, XK_grave, XK_O },
  { XK_Ugrave, XK_grave, XK_U },
  { XK_agrave, XK_grave, XK_a },
  { XK_egrave, XK_grave, XK_e },
  { XK_igrave, XK_grave, XK_i },
  { XK_ograve, XK_grave, XK_o},
  { XK_ugrave, XK_grave, XK_u },

  { XK_Aacute, XK_acute, XK_A },
  { XK_Eacute, XK_acute, XK_E },
  { XK_Iacute, XK_acute, XK_I },
  { XK_Oacute, XK_acute, XK_O },
  { XK_Uacute, XK_acute, XK_U },
  { XK_Yacute, XK_acute, XK_Y },
  { XK_aacute, XK_acute, XK_a },
  { XK_eacute, XK_acute, XK_e },
  { XK_iacute, XK_acute, XK_i },
  { XK_oacute, XK_acute, XK_o},
  { XK_uacute, XK_acute, XK_u },
  { XK_yacute, XK_acute, XK_y },

  { XK_Acircumflex, XK_asciicircum, XK_A },
  { XK_Ecircumflex, XK_asciicircum, XK_E },
  { XK_Icircumflex, XK_asciicircum, XK_I },
  { XK_Ocircumflex, XK_asciicircum, XK_O },
  { XK_Ucircumflex, XK_asciicircum, XK_U },
  { XK_acircumflex, XK_asciicircum, XK_a },
  { XK_ecircumflex, XK_asciicircum, XK_e },
  { XK_icircumflex, XK_asciicircum, XK_i },
  { XK_ocircumflex, XK_asciicircum, XK_o},
  { XK_ucircumflex, XK_asciicircum, XK_u },

  { XK_Adiaeresis, XK_diaeresis, XK_A },
  { XK_Ediaeresis, XK_diaeresis, XK_E },
  { XK_Idiaeresis, XK_diaeresis, XK_I },
  { XK_Odiaeresis, XK_diaeresis, XK_O },
  { XK_Udiaeresis, XK_diaeresis, XK_U },
  { XK_adiaeresis, XK_diaeresis, XK_a },
  { XK_ediaeresis, XK_diaeresis, XK_e },
  { XK_idiaeresis, XK_diaeresis, XK_i },
  { XK_odiaeresis, XK_diaeresis, XK_o},
  { XK_udiaeresis, XK_diaeresis, XK_u },
  { XK_ydiaeresis, XK_diaeresis, XK_y },

  { XK_Aring, XK_degree, XK_A },
  { XK_aring, XK_degree, XK_a },

  { XK_Ccedilla, XK_cedilla, XK_C },
  { XK_ccedilla, XK_cedilla, XK_c },

  { XK_Atilde, XK_asciitilde, XK_A },
  { XK_Ntilde, XK_asciitilde, XK_N },
  { XK_Otilde, XK_asciitilde, XK_O },
  { XK_atilde, XK_asciitilde, XK_a },
  { XK_ntilde, XK_asciitilde, XK_n },
  { XK_otilde, XK_asciitilde, XK_o },
};

// Peica Add 
// temporary method for _VkKeyScan(...)

// Thomas 21.05.2008 insert type SHORT
//const VkScanKeyMap[] = {0x0020, 0x0131, 0x01DE, 0x0133, 0x0134, 0x0135, 0x0137, 
static const SHORT VkScanKeyMap[] = {0x0020, 0x0131, 0x01DE, 0x0133, 0x0134, 0x0135, 0x0137, 
											  0x00DE, 0x0139, 0x0130, 0x0138, 0x01BB, 0x00BC, 0x00BD, 
												0x00BE, 0x00BF, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 
												0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x01BA, 0x00BA, 
												0x01BC, 0x00BB, 0x01BE, 0x01BF, 0x0132, 0x0141, 0x0142, 
												0x0143, 0x0144, 0x0145, 0x0146, 0x0147, 0x0148, 0x0149, 
												0x014A, 0x014B, 0x014C, 0x014D, 0x014E, 0x014F, 0x0150, 
												0x0151, 0x0152, 0x0153, 0x0154, 0x0155, 0x0156, 0x0157, 
												0x0158, 0x0159, 0x015A, 0x00DB, 0x00DC, 0x00DD, 0x0136, 
												0x01BD, 0x00C0, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 
												0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 
												0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 
												0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 
												0x01DB, 0x01DC, 0x01DD, 0x01C0, 0x0208};

// Peica Add

SHORT _VkKeyScan(char keysym)
{
  SHORT vkKey=0;
  
 	if (keysym >= 32 && keysym <= 126){
    vkKey = VkScanKeyMap[keysym - 32];
  }
  else if (keysym >= 160 && keysym <= 255){
    vkKey = -1;
  }
  else{
      vkKey = -1;
  }

  return vkKey;
}
// doKeyboardEvent wraps the system keybd_event function and attempts to find
// the appropriate scancode corresponding to the supplied virtual keycode.

inline void doKeyboardEvent(BYTE vkCode, DWORD flags) {
  vlog.debug("vkCode 0x%x flags 0x%x", vkCode, flags);
  keybd_event(vkCode, MapVirtualKey(vkCode, 0), flags, 0);
}

// KeyStateModifier is a class which helps simplify generating a "fake" press
// or release of shift, ctrl, alt, etc.  An instance of the class is created
// for every key which may need to be pressed or released.  Then either press()
// or release() may be called to make sure that the corresponding key is in the
// right state.  The destructor of the class automatically reverts to the
// previous state.

class KeyStateModifier {
public:
  KeyStateModifier(int vkCode_, int flags_=0)
    : vkCode(vkCode_), flags(flags_), pressed(false), released(false)
  {}
  void press() {
    if (!(GetAsyncKeyState(vkCode) & 0x8000)) {
      doKeyboardEvent(vkCode, flags);
      pressed = true;
    }
  }
  void release() {
    if (GetAsyncKeyState(vkCode) & 0x8000) {
      doKeyboardEvent(vkCode, flags | KEYEVENTF_KEYUP);
      released = true;
    }
  }
  ~KeyStateModifier() {
    if (pressed) {
      doKeyboardEvent(vkCode, flags | KEYEVENTF_KEYUP);
    } else if (released) {
      doKeyboardEvent(vkCode, flags);
    }
  }
  int vkCode;
  int flags;
  bool pressed;
  bool released;
};


// doKeyEventWithModifiers() generates a key event having first "pressed" or
// "released" the shift, ctrl or alt modifiers if necessary.

void doKeyEventWithModifiers(BYTE vkCode, BYTE modifierState, bool down)
{
  KeyStateModifier ctrl(VK_CONTROL);
  KeyStateModifier alt(VK_MENU);
  KeyStateModifier shift(VK_SHIFT);

  if (down) {
    if (modifierState & 2) ctrl.press();
    if (modifierState & 4) alt.press();
    if (modifierState & 1) {
      shift.press(); 
    } else {
      shift.release();
    }
  }
  doKeyboardEvent(vkCode, down ? 0 : KEYEVENTF_KEYUP);
}


win32::SKeyboard::SKeyboard()
{
  oneShift = rfb::win32::osVersion.isPlatformWindows;
  for (int i = 0; i < sizeof(keymap) / sizeof(keymap_t); i++) {
    vkMap[keymap[i].keysym] = keymap[i].vk;
    extendedMap[keymap[i].keysym] = keymap[i].extended;
  }

  // Find dead characters for the current keyboard layout
  // XXX how could we handle the keyboard layout changing?
#if 0
  // Peica do not want to add the dead key feature into this version VNC.
  BYTE keystate[256];
  memset(keystate, 0, 256);
  for (int j = 0; j < sizeof(latin1DeadChars); j++) {
    SHORT s = VkKeyScan(latin1DeadChars[j]);
    if (s != -1) {
      BYTE vkCode = LOBYTE(s);
      BYTE modifierState = HIBYTE(s);
      keystate[VK_SHIFT] = (modifierState & 1) ? 0x80 : 0;
      keystate[VK_CONTROL] = (modifierState & 2) ? 0x80 : 0;
      keystate[VK_MENU] = (modifierState & 4) ? 0x80 : 0;
      rdr::U8 chars[2];
      int nchars = ToAscii(vkCode, 0, keystate, (WORD*)&chars, 0);
      if (nchars < 0) {
        vlog.debug("Found dead key 0x%x '%c'",
                   latin1DeadChars[j], latin1DeadChars[j]);
        deadChars.push_back(latin1DeadChars[j]);
        ToAscii(vkCode, 0, keystate, (WORD*)&chars, 0);
      }
    }
  }
#endif
}

void win32::SKeyboard::keyEvent(rdr::U32 keysym, bool down)
{
  for (int i = 0; i < sizeof(keysymToAscii) / sizeof(keysymToAscii_t); i++) {
    if (keysymToAscii[i].keysym == keysym) {
      keysym = keysymToAscii[i].ascii;
      break;
    }
  }

  if ((keysym >= 32 && keysym <= 126) ||
      (keysym >= 160 && keysym <= 255))
  {
    // ordinary Latin-1 character

#if 0
    // Peica do not detect dead chars
    if (deadKeyAware) {
      // Detect dead chars and generate the dead char followed by space so
      // that we'll end up with the original char.
      for (int i = 0; i < deadChars.size(); i++) {
        if (keysym == deadChars[i]) {
          SHORT dc = VkKeyScan(keysym);
          if (dc != -1) {
            if (down) {
              vlog.info("latin-1 dead key: 0x%x vkCode 0x%x mod 0x%x "
                        "followed by space", keysym, LOBYTE(dc), HIBYTE(dc));
              doKeyEventWithModifiers(LOBYTE(dc), HIBYTE(dc), true);
              doKeyEventWithModifiers(LOBYTE(dc), HIBYTE(dc), false);
              doKeyEventWithModifiers(VK_SPACE, 0, true);
              doKeyEventWithModifiers(VK_SPACE, 0, false);
            }
            return;
          }
        }
      }
    }
#endif
    //  convert keysym to virtual-key
    //SHORT s = VkKeyScan(keysym);
    SHORT s = _VkKeyScan(keysym);

    if (s == -1) {
#if 0
      if (down) {
        // not a single keypress - try synthesizing dead chars.
        for (int j = 0;
             j < sizeof(latin1ToDeadChars) / sizeof(latin1ToDeadChars_t);
             j++) {
          if (keysym == latin1ToDeadChars[j].latin1Char) {
            for (int i = 0; i < deadChars.size(); i++) {
              if (deadChars[i] == latin1ToDeadChars[j].deadChar) {
                SHORT dc = VkKeyScan(latin1ToDeadChars[j].deadChar);
                SHORT bc = VkKeyScan(latin1ToDeadChars[j].baseChar);
                if (dc != -1 && bc != -1) {
                  vlog.info("latin-1 key: 0x%x dead key vkCode 0x%x mod 0x%x "
                            "followed by vkCode 0x%x mod 0x%x",
                            keysym, LOBYTE(dc), HIBYTE(dc),
                            LOBYTE(bc), HIBYTE(bc));
                  doKeyEventWithModifiers(LOBYTE(dc), HIBYTE(dc), true);
                  doKeyEventWithModifiers(LOBYTE(dc), HIBYTE(dc), false);
                  doKeyEventWithModifiers(LOBYTE(bc), HIBYTE(bc), true);
                  doKeyEventWithModifiers(LOBYTE(bc), HIBYTE(bc), false);
                  return;
                }
                break;
              }
            }
            break;
          }
        }       
        vlog.info("ignoring unrecognised Latin-1 keysym 0x%x",keysym);
      }
#endif
      return;      
    }

    BYTE vkCode = LOBYTE(s);
    BYTE modifierState = HIBYTE(s);
    vlog.debug("latin-1 key: 0x%x vkCode 0x%x mod 0x%x down %d",
               keysym, vkCode, modifierState, down);
    doKeyEventWithModifiers(vkCode, modifierState, down);

  } else {

    // see if it's a recognised keyboard key, otherwise ignore it

    if (vkMap.find(keysym) == vkMap.end()) {
      vlog.info("ignoring unknown keysym 0x%x",keysym);
      return;
    }
    BYTE vkCode = vkMap[keysym];
    DWORD flags = 0;
    if (extendedMap[keysym]) flags |= KEYEVENTF_EXTENDEDKEY;
    if (!down) flags |= KEYEVENTF_KEYUP;

    vlog.debug("keyboard key: keysym 0x%x vkCode 0x%x ext %d down %d",
               keysym, vkCode, extendedMap[keysym], down);
 
    doKeyboardEvent(vkCode, flags);
  }
}
