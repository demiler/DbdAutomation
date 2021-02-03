#ifndef EMULATOR_H
#define EMULATOR_H
#include <windows.h>
#include "keys.h"

void pushKey(kbKey key);
void releaseKey(kbKey key);
void pressKey(kbKey key, int delay);

void pushMouseBtn(msKey button);
void releaseMouseBtn(msKey button);
void pressMouseBtn(msKey button, int delay);

#endif
