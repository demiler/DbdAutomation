#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "handlers.h"
#include "utils.h"
#include "logs.h"
#include "sounds.h"
#include "keys.h"
#include "global.h"

#include "automate.c"

static void (*actFuncPtr)(void);

DWORD WINAPI
wrap(LPVOID nothing) {
  actFuncPtr();
}

DWORD WINAPI
thing(LPVOID nothing) {
  scriptsInit();
  HANDLE automate = NULL;
  bool exit = false;
  enum semEv_t lastEv = SE_NOTHING;

  while (!exit) {
    semEv = SE_NOTHING;
    WaitForSingleObject(semaphore, INFINITE);
    //printf("Sem event: %d\n", semEv);

    actFuncPtr = NULL;
    switch (semEv) {
      case SE_APP_FOCUSED:
        if (semEv != lastEv) printf("App focused\n");
        focused = true;
        break;
      case SE_APP_BLURED:
        if (semEv != lastEv) printf("App blured\n");
        stop();
        focused = false;
        break;
      case SE_SCRIPT_RESTART:
        if (semEv != lastEv) printf("Script time restarted\n");
        resetTime();
        break;
      case SE_SCRIPT_CANCEL:
        if (semEv != lastEv) printf("Script canceled\n");
        stop();
        break;
      case SE_SCRIPT_SPECIAL:
        if (semEv != lastEv) printf("Script special\n");
        action();
        break;
      case SE_PROGRAM_CLOSE:
        if (semEv != lastEv) printf("Close program\n");
        exit = true;
        break;
      case SE_SCRIPT_WIGGLE:
        actFuncPtr = wiggle;
        break;
      case SE_SCRIPT_STRUGGLE:
        actFuncPtr = struggle;
        break;
      case SE_SCRIPT_AUTOGEN:
        actFuncPtr = autoGen;
        break;
      case SE_SCRIPT_TOXIC:
        actFuncPtr = becomeToxic;
        break;
    }
    
    //focused = true; //DEBUG ONLY ==========================

    if (focused && actFuncPtr && !active) {
      printf("Running new action\n");
      automate = CreateThread(NULL, 0, wrap, NULL, 0, NULL);
    }
    else if (actFuncPtr) {
      printf("New action not executed because ");
      if (!focused)
        printf("app is not in focus, ");
      if (active)
        printf("another action is already active");
      printf("\n");
    }

    if (lastEv == semEv)
      printf("I");

    lastEv = semEv;
  }

  stop();
  closeProgram();
  return 0;
}

void
start() {
  HANDLE seconds = CreateThread(NULL, 0, thing, NULL, 0, NULL);

  //start message loop
	MSG msg;
	GetMessage(&msg, NULL, 0, 0);
}

int
main(void) {
  loadConsole();
  setHooks();
  start();
  return 0;
}
