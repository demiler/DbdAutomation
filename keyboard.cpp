#include "keyboard.h"

LRESULT CALLBACK KeyboardHandler::hookCallback(int nCode, WPARAM action, LPARAM rawData)
{
    if (nCode < 0)
        return CallNextHookEx(NULL, nCode, action, rawData);

    KBDLLHOOKSTRUCT data = *(static_cast<KBDLLHOOKSTRUCT*>(rawData));

    if (data.flags == LLKHF_INJECTED && ignoreInjections)
        return CallNextHookEx(NULL, nCode, action, rawData);

    Keyboard::Button actionCode = data.vkCode;
    if (action == WM_KEYUP || action == WM_SYSKEYUP)
        actionCode |= Keyboard::State::up;
    else if (action == WM_KEYDOWN || action == WM_KEYDOWN)
        actionCode |= Keyboard::State::down;

    auto key = getKeyOnly(actionCode);
    buttonsState[key] = isUpState(actionCode);

    //by winapi documentation to lock button update from hook, you need to return non-zero
    return lockUpdate[key] ? 1 : CallNextHookEx(NULL, nCode, action, rawData);
}

KeyboardHandler::KeyboardHandler(bool ignoreInjections) :
    BasicDeviceHandler::BasicDeviceHandler(ignoreInjections)
{
    hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHandler::hookCallback, NULL, 0);
    if (!hook)
        throw "unable to create mouse hook";
}

KeyboardHandler::~KeyboardHandler()
{
    UnhookWindowsHookEx(hook);
}
