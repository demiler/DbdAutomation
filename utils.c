#include <Windows.h>
#include <stdio.h>
#include "utils.h"
#include "logs.h"
#include "handlers.h"
#include "global.h"

/*============================VARIABLES============================*/
static HANDLE hCons;               //console handle
static DWORD oldConsMode;          //old console mode
static HHOOK kbHook;               //keyboard low level hook;
static HHOOK msHook;               //mouse low level hook;
static HWINEVENTHOOK evHook;       //event hook
 
/*===============================FUNCTIONS================================*/
void 
closeProgram(void) {
  printf("Exiting...\n");
  //clear possible console input
  INPUT_RECORD irInBuf[4096];
  DWORD cNumRead;
  ReadConsoleInput(hCons, irInBuf, 4096, &cNumRead);

  //resotre old console mod
  SetConsoleMode(hCons, oldConsMode);
  //release hook
	UnhookWindowsHookEx(kbHook);
	UnhookWindowsHookEx(msHook);
  UnhookWinEvent(evHook);
  CloseHandle(semaphore);

  //terminate program
  exit(0);
}


void
loadConsole(void) {
  // Get the standard input handle.
  hCons = GetStdHandle(STD_INPUT_HANDLE);
  if (hCons == INVALID_HANDLE_VALUE) exit(1);

  // Save the current input mode, to be restored on exit.
  if (!GetConsoleMode(hCons, &oldConsMode)) exit(2);

  // Enable the window and mouse input events.
  DWORD fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
  if (!SetConsoleMode(hCons, fdwMode)) exit(3);

  printf("Console output enabled successfully\n");
}

void
setHooks(void) {
  semaphore = CreateSemaphoreA(NULL, 0, 1, NULL);

	kbHook = SetWindowsHookEx(WH_KEYBOARD_LL, kbHookCallback, NULL, 0);
  if (!kbHook)
    error("Can't set WH_KEYBOARD_LL hook", 4);
  //---------

  msHook = SetWindowsHookEx(WH_MOUSE_LL, msHookCallback, NULL, 0);
  if (!msHook)
    error("Can't set WH_MOUSE_LL hook", 5);
  //---------


  HWINEVENTHOOK hEvHook = SetWinEventHook(EVENT_MIN, EVENT_MAX,
    GetModuleHandle(NULL), (WINEVENTPROC)hEvHandler, 0, 0, 
    WINEVENT_OUTOFCONTEXT);
  if (!hEvHook)
    error("Can't set event hook: %d", 11, GetLastError());

  /*
  //load dll and function from it for next hook
  HINSTANCE dll = LoadLibraryA("hooks.dll"); 
  if (!dll)
    error("Dll could not be loaded: %d\n", 7, GetLastError());
  //WINEVENTPROC hEvHandler = (WINEVENTPROC) GetProcAddress(dll, "hEvHandler");

  HOOKPROC fcHookCallback = (HOOKPROC) GetProcAddress(dll, "fcHookCallback");
  if (!fcHookCallback)
    error("Hook func could not be loaded: %d\n", 9, GetLastError());
  fcHook = SetWindowsHookEx(WH_SHELL, fcHookCallback, dll, 0);

  if (!fcHook)
    error("Can't set WH_CBT hook: %d", 6, GetLastError());
  //---------
  */

  printf("Hooks loaded successfully\n");
}

int 
random(int from, int to) {
  return rand() % (to - from + 1) + from;
}

time_t
millis(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;

}

