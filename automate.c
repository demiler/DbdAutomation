#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "sounds.h"
#include "utils.h"
#include "emulator.h"

//X 1) printf, beep
//X 2) run for n seconds then warning 
//X 3) run for n + p seconds then disable
//X 4) press 'a' and 'd' buttons while running
//_ 5) reset by middle mouse button
//_ 6) stop by 'shift' or 'ctrl'
void wiggle(void) {
  printf("Wiggle script active\n");
  makeSound(S_SCRIPT_ENABLED);

  const time_t stopTime = 10000; //10ms
  const time_t wrngTime = 8000; //warning time 8ms

  time_t runTime = 0, startTime = millis();
  bool warned = false;

  while (runTime < stopTime) {
    pressKey(KBK_A, 30);
    pressKey(KBK_D, 30);
    Sleep(random(10, 50));

    runTime = millis() - startTime;
    if (runTime > wrngTime && !warned) {
      warned = true;
      makeSound(S_SCRIPT_ABOUT_TO_END);
    }
  }

  makeSound(S_SCRIPT_DISABLED);
  printf("Wiggle script disabled\n");
}

//_ 1) printf, beep
//_ 2) run for n seconds then warning 
//_ 3) run for n + p seconds then disable
//_ 4) press 'space' button while running
//_ 5) reset by middle mouse button
//_ 6) stop by 'shift' or 'ctrl'
void struggle(void) {
  printf("Struggle script active\n");
  makeSound(S_SCRIPT_ENABLED);

  const time_t stopTime = 30000;
  const time_t wrngTime = stopTime - 5000; //warning time

  time_t runTime = 0, startTime = millis();
  bool warned = false;

  while (runTime < stopTime) {
    pressKey(KBK_SPACE, 30);
    Sleep(random(10, 50));

    runTime = millis() - startTime;
    if (runTime > wrngTime && !warned) {
      warned = true;
      makeSound(S_SCRIPT_ABOUT_TO_END);
    }
  }

  makeSound(S_SCRIPT_DISABLED);
  printf("Struggle script disabled\n");
}
