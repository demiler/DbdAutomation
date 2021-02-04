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

DWORD WINAPI
thing(LPVOID nothing) {
  while (semEv != SE_PROGRAM_CLOSE) {
    semEv = SE_NOTHING;
    WaitForSingleObject(semaphore, INFINITE);
    printf("Sem event: %d\n", semEv);
  }

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
