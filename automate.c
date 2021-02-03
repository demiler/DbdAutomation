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

//X 1) printf, beep
//X 2) run for n seconds then warning 
//X 3) run for n + p seconds then disable
//X 4) press 'space' button while running
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

//1)_ printf
//2)_ disable if shift pressed
//3)_ detect if lmb is pressed
//4)_ press ctrl or lmb while running
//5)_ run until ctrl or lmb is pressed
//    run at least for few sec if neither 
//    pressed.
void becomeToxic(void) {
  printf("Become toxic script active\n");

  const time_t stopTime = 500;
  const time_t tBagTime = 60;
  const time_t clickTime = 30;
  const bool tBag = true;

  if (tBag)
    pressKey(KBK_CTRL, 125);

  time_t runTime = 0, startTime = millis();
  while (runTime < stopTime) {
    if (tBag) {
      pushKey(KBK_CTRL);
      Sleep(tBagTime);
      releaseKey(KBK_CTRL);
      Sleep(tBagTime);
    }
    else {
      pushMouseBtn(MSK_RIGHT);
      Sleep(clickTime);
      releaseMouseBtn(MSK_RIGHT);
      Sleep(clickTime);
    }

    runTime = millis() - startTime;
  }

  printf("Become toxic script disable\n");
}
