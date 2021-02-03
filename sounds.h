#ifndef SOUNDS_H
#define SOUNDS_H
#include <windows.h>

typedef enum sound_t {
  S_PROGRAM_START,
  S_PROGRAM_CLOSE,
  S_PROGRAM_ACTIVE,
  S_PROGRAM_INACTIVE,
  S_SCRIPT_ENABLED,
  S_SCRIPT_ABOUT_TO_END,
  S_SCRIPT_DISAVLED,
} sound_t;

struct BeepData {
  DWORD freq, duration;
  int count;
};

void makeSound(sound_t sound);

#endif
