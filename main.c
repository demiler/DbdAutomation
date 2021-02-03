#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "handlers.h"
#include "utils.h"
#include "logs.h"
#include "sounds.h"
#include <math.h>

void
start() {
  //start message loop
	MSG msg;
	GetMessage(&msg, NULL, 0, 0);
}

int
main(void) {
  loadConsole();
  //setHooks();
  //start();

  makeSound(S_PROGRAM_START);
  Sleep(1000);
  makeSound(S_PROGRAM_CLOSE);
  Sleep(1000);

  //HANDLE mlHandler = CreateThread(NULL, 0, messageLoop, NULL, 0, NULL);
 
  return 0;
}
