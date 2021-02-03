#ifndef PHANDLERS_H
#define PHANDLERS_H
#include <Windows.h>

extern LRESULT CALLBACK
kbHookCallback(int nCode, WPARAM action, LPARAM lParam);

extern LRESULT CALLBACK
msHookCallback(int nCode, WPARAM action, LPARAM lParam);

extern void 
hEvHandler(
  HWINEVENTHOOK hWinEventHook,
  DWORD event,
  HWND hwnd,
  LONG idObject,
  LONG idChild,
  DWORD idEventThread,
  DWORD dwmsEventTime
);

#endif
