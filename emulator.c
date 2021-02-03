#include "emulator.h"

void pushKey(kbKey key) {
  static INPUT inp = { .type = INPUT_KEYBOARD, 0 };
  inp.ki.wVk = key;
  SendInput(1, &inp, sizeof(inp));
}

void releaseKey(kbKey key) {
  static INPUT inp = { .type = INPUT_KEYBOARD,
    .ki.dwFlags = KEYEVENTF_KEYUP, 0 };
  inp.ki.wVk = key;
  SendInput(1, &inp, sizeof(inp));
}

void pressKey(kbKey key, int delay) {
  static INPUT inp[2] = {{ .type = INPUT_KEYBOARD, 0 },
    { .type = INPUT_KEYBOARD, .ki.dwFlags = KEYEVENTF_KEYUP, 0 }};

  inp[0].ki.wVk = inp[1].ki.wVk = key;
  if (delay > 0) {
    SendInput(1, inp, sizeof(*inp));
    Sleep(delay);
    SendInput(1, inp + 1, sizeof(*inp));
  }
  else
    SendInput(2, inp, sizeof(*inp));
}

void pushMouseBtn(msKey button) {
  static INPUT inp = { .type = INPUT_MOUSE, 0 };

  switch (button) {
    case MSK_LEFT: //left
      inp.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
      break;
    case MSK_MIDDLE: //middle
      inp.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
      break;
    case MSK_RIGHT: //right
      inp.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
      break;
    case MSK_FORWARD: //forward
      inp.mi.dwFlags = MOUSEEVENTF_XDOWN;
      inp.mi.mouseData = XBUTTON2;
      break;
    case MSK_BACKWARD: //backward
      inp.mi.dwFlags = MOUSEEVENTF_XDOWN;
      inp.mi.mouseData = XBUTTON1;
      break;
  }
  SendInput(1, &inp, sizeof(inp));
}

void releaseMouseBtn(msKey button) {
  static INPUT inp = { .type = INPUT_MOUSE, 0 };

  switch (button) {
    case MSK_LEFT: //left
      inp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
      break;
    case MSK_MIDDLE: //middle
      inp.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
      break;
    case MSK_RIGHT: //right
      inp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
      break;
    case MSK_FORWARD: //forward
      inp.mi.dwFlags = MOUSEEVENTF_XUP;
      inp.mi.mouseData = XBUTTON2;
      break;
    case MSK_BACKWARD: //backward
      inp.mi.dwFlags = MOUSEEVENTF_XUP;
      inp.mi.mouseData = XBUTTON1;
      break;
  }
  SendInput(1, &inp, sizeof(inp));
}

void pressMouseBtn(msKey button, int delay) {
  static INPUT inp[2] = { { .type = INPUT_MOUSE, 0 }, 
    { .type = INPUT_MOUSE, 0 } };

  switch (button) {
    case MSK_LEFT: //left
      inp[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
      inp[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
      break;
    case MSK_MIDDLE: //middle
      inp[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
      inp[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
      break;
    case MSK_RIGHT: //right
      inp[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
      inp[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
      break;
    case MSK_FORWARD: //forward
      inp[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
      inp[1].mi.dwFlags = MOUSEEVENTF_XUP;
      inp[0].mi.mouseData = inp[1].mi.mouseData = XBUTTON2;
      break;
    case MSK_BACKWARD: //backward
      inp[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
      inp[1].mi.dwFlags = MOUSEEVENTF_XUP;
      inp[0].mi.mouseData = inp[1].mi.mouseData = XBUTTON1;
      break;
  }

  if (delay > 0) {
    SendInput(1, inp, sizeof(*inp));
    Sleep(delay);
    SendInput(1, inp + 1, sizeof(*inp));
  }
  else
    SendInput(2, inp, sizeof(*inp));
}
