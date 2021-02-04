#include <stdio.h>
#include <stdbool.h>
#include "handlers.h"
#include "utils.h"
#include "global.h"
#include "keys.h"

//nCode -- is action valid (HC_ACTION)
//lParam -- pointer to struct with keyboard data
//wParam -- action type

//kbHook callback, called every time key is pressed.
LRESULT CALLBACK
kbHookCallback(int nCode, WPARAM action, LPARAM lParam) {
  //just procceed to next hook (documentation requirement)
	if (nCode < 0) 
    //first argument is ignored (msdn documentation)
    return CallNextHookEx(NULL, nCode, action, lParam);

  KBDLLHOOKSTRUCT kbData = *((KBDLLHOOKSTRUCT*)lParam);

  if (action == WM_KEYUP) {
    switch (kbData.vkCode) {
      case KBK_Q: 
        semEv = SE_PROGRAM_CLOSE; break;
      case KBK_Z:
        semEv = SE_SCRIPT_AUTOGEN; break;
    }
  } else if (action == WM_KEYDOWN && kbData.flags != LLKHF_INJECTED) {
    //printf("pressed: %x\n", kbData.flags);
    if (kbData.vkCode == KBK_SHIFT || kbData.vkCode == KBK_CTRL)
      semEv = SE_SCRIPT_CANCEL;
  }

  if (semEv != SE_NOTHING)
    ReleaseSemaphore(semaphore, 1, NULL);

  if (showKeys) {
    switch (action) {
      case WM_KEYDOWN:    printf(" WM_KEYDOWN    "); break;
      case WM_KEYUP:      printf(" WM_KEYUP      "); break;
      case WM_SYSKEYDOWN: printf(" WM_SYSKEYDOWN "); break;
      case WM_SYSKEYUP:   printf(" WM_SYSKEYUP   "); break;
      default:            printf(" UNKNOWN       ");
    }
    printf("%5d | %5d\n", kbData.vkCode, kbData.scanCode);
  }

  switch (action) {
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
      kbKeys[kbData.vkCode] = true;
      break;
    case WM_SYSKEYUP:
    case WM_KEYUP:
      kbKeys[kbData.vkCode] = false;
      break;
  }

	// call the next hook in the hook chain.
  //This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(NULL, nCode, action, lParam);
}

//=======================================

LRESULT CALLBACK
msHookCallback(int nCode, WPARAM action, LPARAM lParam) {
	if (nCode < 0 || action == WM_MOUSEMOVE) //not mouse msg
    return CallNextHookEx(NULL, nCode, action, lParam);

  bool validEvent = true;
  MSLLHOOKSTRUCT data = *((MSLLHOOKSTRUCT*)lParam);

	if (data.flags == 0x1/*LLMHF_INJECTED*/)
    return CallNextHookEx(NULL, nCode, action, lParam);

  switch (action) {
    case WM_LBUTTONDOWN: msKeys[MSK_LEFT] = true; break;
    case WM_LBUTTONUP:   msKeys[MSK_LEFT] = false; break;
    case WM_RBUTTONDOWN: msKeys[MSK_RIGHT] = true; break;
    case WM_RBUTTONUP:   msKeys[MSK_RIGHT] = false; break;
    case WM_MBUTTONDOWN: msKeys[MSK_MIDDLE] = true; break;
    case WM_MBUTTONUP:   msKeys[MSK_MIDDLE] = false; break;
    case WM_XBUTTONDOWN:
      if (HIWORD(data.mouseData) == XBUTTON1)
        msKeys[MSK_BACKWARD] = true;
      else
        msKeys[MSK_FORWARD] = true;
      break;
    case WM_XBUTTONUP:
      if (HIWORD(data.mouseData) == XBUTTON1)
        msKeys[MSK_BACKWARD] = false;
      else
        msKeys[MSK_FORWARD] = false;
      break;

    default:
      validEvent = false;
  }

  if (validEvent) {
    if (msKeys[MSK_RIGHT] && active)
      semEv = SE_SCRIPT_SPECIAL;
    else if (msKeys[MSK_MIDDLE])
      semEv = active ? SE_SCRIPT_RESTART : SE_SCRIPT_STRUGGLE;
    else if (msKeys[MSK_FORWARD])
      semEv = SE_SCRIPT_WIGGLE;
    else if (msKeys[MSK_BACKWARD])
      semEv = SE_SCRIPT_TOXIC;

    if (semEv != SE_NOTHING)
      ReleaseSemaphore(semaphore, 1, NULL);
  }

return CallNextHookEx(NULL, nCode, action, lParam);
}

//=======================================

void 
hEvHandler(
  HWINEVENTHOOK hWinEventHook,
  DWORD event,
  HWND hwnd,
  LONG idObject,
  LONG idChild,
  DWORD idEventThread,
  DWORD dwmsEventTime
) {
  if (event == EVENT_OBJECT_FOCUS) {
    if (hwnd == NULL)
      printf("Focus event wasn't caused by window\n");
    else {
      TCHAR title[256];
      int len = GetWindowTextA(hwnd, title, 256);
      if (!len) {
        //printf("Focused window has no title\n");
        if (focused)
          semEv = SE_APP_BLURED;
      }
      else {
        //printf("Windows '%hs' focused\n", title);
        if (cmpTcharStr(title, AppName))
          semEv = SE_APP_FOCUSED;
        else if (focused)
          semEv = SE_APP_BLURED;
      }
    }

    if (semEv != SE_NOTHING)
      ReleaseSemaphore(semaphore, 1, NULL);
  }
}
