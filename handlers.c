#include "handlers.h"
#include "utils.h"
#include "global.h"
#include <stdio.h>

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
  if (action == WM_KEYUP && kbData.vkCode == 0x51) //q code
    closeProgram();

  if (showKeys) {
    switch (action) {
      case WM_KEYDOWN:    printf("%15s ", "WM_KEYDOWN"); break;
      case WM_KEYUP:      printf("%15s ", "WM_KEYUP"); break;
      case WM_SYSKEYDOWN: printf("%15s ", "WM_SYSKEYDOWN"); break;
      case WM_SYSKEYUP:   printf("%15s ", "WM_SYSKEYUP"); break;
      default:            printf("%15s ", "UNKNOWN");
    }
    printf("%5d | %5d\n", kbData.vkCode, kbData.scanCode);
  }

	// call the next hook in the hook chain.
  //This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(NULL, nCode, action, lParam);
}

//=======================================

LRESULT CALLBACK
msHookCallback(int nCode, WPARAM action, LPARAM lParam) {
	if (nCode < 0) //not mouse msg
    return CallNextHookEx(NULL, nCode, action, lParam);

  if (action == WM_RBUTTONUP)
    printf("RMP is up\n");
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
      printf("focus event wasn't caused by window\n");
    else {
      TCHAR title[256];
      int len = GetWindowTextA(hwnd, title, 256);
      if (!len) {
        printf("focused window has no title\n");
      }
      else {
        printf("window '%hs' focused\n", title);
      }
    }
  }
}
