#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "handlers.h"
#include "utils.h"
#include "logs.h"
#include "sounds.h"
#include <time.h>

#include "automate.c"

void
start() {
  //HANDLE mlHandler = CreateThread(NULL, 0, messageLoop, NULL, 0, NULL);

  //start message loop
	MSG msg;
	GetMessage(&msg, NULL, 0, 0);
}

int
main(void) {
  loadConsole();
  //setHooks();
  //start();

  Sleep(2000);
  struggle();

  return 0;
}
