#include <stdbool.h>
#include <shlwapi.h>
#include "sounds.h"
#include "logs.h"

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

bool _asyncPlayWav(LPCTSTR wav, int beeps) {
  BOOL pOk, fOk = PathFileExists(wav);

  if (fOk == TRUE)
    pOk = PlaySound(wav, NULL, SND_FILENAME | SND_ASYNC);
  if (fOk == FALSE || pOk == FALSE) {
    _asyncBeep(400, 300 / beeps, beeps);
    warning("Can't %s %hs", !fOk ? "find" : "play", wav);
    return true;
  }
  return false;
}

bool _asyncScriptEnabled(void) {
  static const TCHAR wav[] = TEXT("ScriptEnabled.wav");
  BOOL pOk, fOk = PathFileExists(wav);

  if (fOk == TRUE)
    pOk = PlaySound(wav, NULL, SND_FILENAME | SND_ASYNC);
  if (fOk == FALSE || pOk == FALSE) {
    _asyncBeep(400, 200, 1);
    warning("Can't %s %hs", !fOk ? "find" : "play", wav);
    return true;
  }
  return false;
}

bool _asyncScriptWarning(void) {
  static const TCHAR wav[] = TEXT("ScriptWarning.wav");
  BOOL pOk, fOk = PathFileExists(wav);

  if (fOk == TRUE)
    pOk = PlaySound(wav, NULL, SND_FILENAME | SND_ASYNC);
  if (fOk == FALSE || pOk == FALSE) {
    _asyncBeep(400, 130, 2);
    warning("Can't %s %hs", !fOk ? "find" : "play", wav);
    return true;
  }
  return false;
}

bool _asyncScriptDisabled(void) {
  static const TCHAR wav[] = TEXT("ScriptDisabled.wav");
  BOOL pOk, fOk = PathFileExists(wav);

  if (fOk == TRUE)
    pOk = PlaySound(wav, NULL, SND_FILENAME | SND_ASYNC);
  if (fOk == FALSE || pOk == FALSE) {
    _asyncBeep(400, 100, 3);
    warning("Can't %s %hs", !fOk ? "find" : "play", wav);
    return true;
  }
  return false;
}

void makeSound(sound_t sound) {
  static bool isBeep = false;
  if (!isBeep) //stop any wav files
    PlaySound(NULL, 0, 0);

  switch (sound) {
    case S_PROGRAM_START:
      isBeep = true;
      _asyncBeep(750, 300, 2);
      break;

    case S_PROGRAM_CLOSE:
      isBeep = true;
      _asyncBeep(350, 100, 3);
      break;

    case S_PROGRAM_ACTIVE:
      isBeep = true;
      _asyncActiveBeep();
      break;

    case S_PROGRAM_INACTIVE:
      isBeep = true;
      _asyncInactiveBeep();
      break;

    case S_SCRIPT_ENABLED:
      isBeep = _asyncScriptEnabled();
      break;

    case S_SCRIPT_ABOUT_TO_END:
      isBeep = _asyncScriptWarning();
      break;

    case S_SCRIPT_DISABLED:
      isBeep = _asyncScriptDisabled();
      break;
  }
}
