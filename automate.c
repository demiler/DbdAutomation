#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "sounds.h"
#include "utils.h"
#include "emulator.h"
#include "keys.h"
#include "global.h"
#include "semevtype.h"

/*===============================VARS AND CONSTS=============================*/
static const time_t wiggleStopTime    = 10000;
static const time_t struggleStopTime  = 10000;
static const time_t autoGenStopTime   = 10000;
static const time_t warningDelayTime  = 5000;
static time_t startTime, runTime;
static HANDLE stopSem;
static enum stopSemEv_t ssemEv;
/*==================================FUNCTIONS================================*/
void scriptsInit() {
  stopSem = CreateSemaphoreA(NULL, 0, 1, NULL);
  ssemEv = SSE_NOTHING;
}

void resetTime(void) {
  startTime = millis();
  runTime = 0;
  ssemEv = SSE_RESET;
  ReleaseSemaphore(stopSem, 1, NULL);
}

void stop(void) {
  ssemEv = SSE_STOP;
  ReleaseSemaphore(stopSem, 1, NULL);
}

void action(void) {
  ssemEv = SSE_ACTION;
  ReleaseSemaphore(stopSem, 1, NULL);
}

//X 1) printf, beep
//X 2) run for n seconds then warning 
//X 3) run for n + p seconds then disable
//X 4) press 'a' and 'd' buttons while running
//_ 5) reset by middle mouse button
//_ 6) stop by 'shift' or 'ctrl'
void wiggle(void) {
  printf("Wiggle script active\n");
  makeSound(S_SCRIPT_ENABLED);

  const time_t wrngTime = wiggleStopTime - warningDelayTime;
  bool warned = false;
  active = true;

  ssemEv = SSE_NOTHING;
  startTime = millis();
  runTime = 0;

  while (runTime < wiggleStopTime) {
    pressKey(KBK_A, 30);
    pressKey(KBK_D, 30);
    //Sleep(random(10, 50));
    DWORD semRes = 
      WaitForSingleObject(stopSem, random(10, 50));

    if (semRes == WAIT_OBJECT_0) {
      if (ssemEv == SSE_STOP)
        break;
      else if (ssemEv == SSE_RESET)
        warned = false;
      ssemEv = SSE_NOTHING;
    }

    runTime = millis() - startTime;

    if (runTime > wrngTime && !warned) {
      warned = true;
      makeSound(S_SCRIPT_ABOUT_TO_END);
    }
  }

  active = false;
  printf("Wiggle script disabled\n");
  makeSound(S_SCRIPT_DISABLED);
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

  const time_t wrngTime = struggleStopTime - warningDelayTime;
  bool warned = false;
  active = true;

  ssemEv = SSE_NOTHING;
  startTime = millis();
  runTime = 0;

  while (runTime < struggleStopTime) {
    pressKey(KBK_SPACE, 30);
    //Sleep(random(10, 50));
    DWORD semRes =
      WaitForSingleObject(stopSem, random(10, 50));

    if (semRes == WAIT_OBJECT_0) {
      if (ssemEv == SSE_STOP)
        break;
      else if (ssemEv == SSE_RESET)
        warned = false;
      ssemEv = SSE_NOTHING;
    }

    runTime = millis() - startTime;

    if (runTime > wrngTime && !warned) {
      warned = true;
      makeSound(S_SCRIPT_ABOUT_TO_END);
    }
  }

  active = false;
  printf("Struggle script disabled\n");
  makeSound(S_SCRIPT_DISABLED);
}

//1)X printf
//2)X disable if shift pressed
//3)X detect if lmb is pressed
//4)X press ctrl or lmb while running
//5)X run until ctrl or lmb is pressed
//    run at least for few sec if neither 
//    pressed.
void becomeToxic(void) {
  if (kbKeys[KBK_SHIFT]) return;

  printf("Become toxic script active\n");

  const time_t stopTime = 500;
  const time_t tBagTime = 60;
  const time_t clickTime = 30;
  const bool tBag = !msKeys[MSK_LEFT];
  active = true;

  if (tBag)
    pressKey(KBK_CTRL, 125);

  ssemEv = SSE_NOTHING;
  startTime = millis();
  runTime = 0;

  while (runTime < stopTime || msKeys[MSK_LEFT]) {
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

    DWORD semRes =
      WaitForSingleObject(stopSem, 0);
    
    if (semRes == WAIT_OBJECT_0 && ssemEv == SSE_STOP) {
      break;
      ssemEv = SSE_NOTHING;
    }

    runTime = millis() - startTime;
  }

  active = false;
  printf("Become toxic script disable\n");
}

//1)X printf, beep
//2)X wait 500ms if shift or ctrl is pressed
//3)X Hold lmb while running
//4)X run for n sec then warning
//5)X run for n+p sec then disable
//6)_ disable if shift or ctrl is pressed
//7)_ if lmb is pressed press 'space' button
//8)_ reset if mmb is pressed 
void autoGen(void) {
  if (kbKeys[KBK_CTRL] || kbKeys[KBK_SHIFT])
    Sleep(300);
  if (kbKeys[KBK_CTRL] || kbKeys[KBK_SHIFT])
    return;

  printf("AutoGen script is active\n");
  makeSound(S_SCRIPT_ENABLED);

  const time_t wrngTime = autoGenStopTime - warningDelayTime;
  bool warned = false;
  active = true;

  ssemEv = SSE_NOTHING;
  startTime = millis();
  runTime = 0;

  pushMouseBtn(MSK_LEFT);
  while (runTime < autoGenStopTime) {
    //Sleep(50);
    DWORD semRes =
      WaitForSingleObject(stopSem, 500);

    if (semRes == WAIT_OBJECT_0) {
      if (ssemEv == SSE_STOP) {
        ssemEv = SSE_NOTHING;
        break;
      }

      switch (ssemEv) {
        case SSE_RESET: warned = false; break;
        case SSE_ACTION: pressKey(KBK_SPACE, 40); break;
      }
      ssemEv = SSE_NOTHING;
    }

    runTime = millis() - startTime;
    if (runTime > wrngTime && !warned) {
      warned = true;
      makeSound(S_SCRIPT_ABOUT_TO_END);
    }
  }
  releaseMouseBtn(MSK_LEFT);

  active = false;
  printf("AutoGen script is disabled\n");
  makeSound(S_SCRIPT_DISABLED);
}
