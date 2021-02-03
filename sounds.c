#include <stdbool.h>
#include "sounds.h"
#include <stdio.h>

DWORD WINAPI _doBeep(LPVOID data) {
  struct BeepData _data = *((struct BeepData*)data);
  for (int i = 0; i < _data.count; i++)
    Beep(_data.freq, _data.duration);
}

HANDLE _asyncBeep(DWORD freq, DWORD duration, int count) {
  static struct BeepData data;
  data.freq = freq;
  data.duration = duration;
  data.count = count;
  return CreateThread(NULL, 0, _doBeep, &data, 0, NULL);
}

DWORD WINAPI _doInactiveBeep(LPVOID n) {
  Beep(3615, 100);
  Beep(1827, 100);
  Beep(886, 100);
}

HANDLE _asyncInactiveBeep(void) {
  return CreateThread(NULL, 0, _doInactiveBeep, NULL, 0, NULL);
}

DWORD WINAPI _doActiveBeep(LPVOID n) {
  Beep(886, 100);
  Beep(1827, 100);
  Beep(3615, 100);
}

HANDLE _asyncActiveBeep(void) {
  return CreateThread(NULL, 0, _doActiveBeep, NULL, 0, NULL);
}

void makeSound(sound_t sound) {
  static DWORD beepThread = 0;
  static bool isBeep = false;

  switch (sound) {
    case S_PROGRAM_START:
      _asyncBeep(750, 300, 2);
      break;
    case S_PROGRAM_CLOSE:
      _asyncBeep(350, 100, 3);
      break;
    case S_PROGRAM_ACTIVE:
      _asyncActiveBeep();
      break;
    case S_PROGRAM_INACTIVE:
      _asyncInactiveBeep();
      break;
    case S_SCRIPT_ENABLED:
      PlaySound(TEXT("ScriptEnabled.wav"), NULL, SND_FILENAME | SND_ASYNC);
      break;
    case S_SCRIPT_ABOUT_TO_END:
      PlaySound(TEXT("ScriptWarning.wav"), NULL, SND_FILENAME | SND_ASYNC);
      break;
    case S_SCRIPT_DISABLED:
      PlaySound(TEXT("ScriptDisabled.wav"), NULL, SND_FILENAME | SND_ASYNC);
      break;
  }
}
