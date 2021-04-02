#include "mouseHandler.h"

unsigned MouseHandler::fromVirtualCode(WPARAM action, MSLLHOOKSTRUCT data)
{
    MouseHandler::Button result;

    switch (action) {
        case WM_MOUSEMOVE:
            result = MouseHandler::Button::movment;
            break;
        case WM_LBUTTONDOWN:
            result = MouseHandler::Button::left | MouseHandler::State::down;
            break;
        case WM_LBUTTONUP:
            result = MouseHandler::Button::left | MouseHandler::State::up;
            break;
        case WM_RBUTTONDOWN:
            result = MouseHandler::Button::right | MouseHandler::State::down;
            break;
        case WM_RBUTTONUP:
            result = MouseHandler::Button::right | MouseHandler::State::up;
            break;
        case WM_MBUTTONDOWN:
            result = MouseHandler::Button::middle | MouseHandler::State::down;
            break;
        case WM_MBUTTONUP:
            result = MouseHandler::Button::middle | MouseHandler::State::up;
            break;
        case WM_XBUTTONDOWN:
            result = MouseHandler::State::down;
            result |= (HIWORD(data.mouseData) == XBUTTON1) ?
                mouseHandler::Button::backward :
                mouseHandler::Button::forward;
            break;
        case WM_XBUTTONUP:
            result = MouseHandler::State::up;
            result |= (HIWORD(data.mouseData) == XBUTTON1) ?
                mouseHandler::Button::backward :
                mouseHandler::Button::forward;
            break;
        default:
            result = MouseHandler::Button::unknown;
    }

    return result;
}

LRESULT CALLBACK MouseHandler::hookCallback(int nCode, WPARAM action, LPARAM rawData)
{
    //nCode < 0 - not mouse event
    if (nCode < 0 || (skipMouseMovment && action == WM_MOUSEMOVE))
        return CallNextHookEx(NULL, nCode, action, rawData);

    MSLLHOOKSTRUCT data = *(std::static_cast<MSLLHOOKSTRUCT*>(rawData));

    if (data.flags == 0x1/*LLMHF_INJECTED*/ && ignoreInjections)
        return CallNextHookEx(NULL, nCode, action, rawData);

    auto actionCode = fromVirtualCode(action, data);
    if (actionCode == MouseHandler::Button::unknown)
        return CallNextHookEx(NULL, nCode, action, rawData);

    callIfPresent(actionCode);

    auto key = getKeyOnly(actionCode);
    buttonsState[key] = isUpState(actionCode);

    //by winapi documentation to lock button update from hook, you need to return non-zero
    return lockUpdate[key] ? 1 : CallNextHookEx(NULL, nCode, action, rawData);
}

MouseHandler::MouseHandler(bool skipMouseMovment, bool ignoreInjections) :
    skipMouseMovment(skipMouseMovment),
    BasicDeviceHandler::BasicDeviceHandler(ignoreInjections)
{
    hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHandler::hookCallback, NULL, 0);
    if (!hook)
        throw "unable to create mouse hook";
}

MouseHandler::~MouseHandler()
{
    UnhookWindowsHookEx(hook);
}

void MouseHandler::pushKey(Button key)
{
    switch (getKeyOnly(key)) {
        MouseHandler::left:
            inp.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            break;
        MouseHandler::right:
            inp.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            break;
        MouseHandler::middle:
            inp.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            break;
        MouseHandler::forward:
            inp.mi.dwFlags = MOUSEEVENTF_XDOWN;
            inp.mi.mouseData = XBUTTON2;
            break;
        MouseHandler::backward:
            inp.mi.dwFlags = MOUSEEVENTF_XDOWN;
            inp.mi.mouseData = XBUTTON1;
            break;
    }
    SendInput(1, &inp, sizeof(inp));
}

void MouseHandler::releaseKey(Button key)
{
    switch (getKeyOnly(key)) {
        case MouseHandler::left: //left
            inp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            break;
        case MouseHandler::middle: //middle
            inp.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            break;
        case MouseHandler::right: //right
            inp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            break;
        case MouseHandler::forward: //forward
            inp.mi.dwFlags = MOUSEEVENTF_XUP;
            inp.mi.mouseData = XBUTTON2;
            break;
        case MouseHandler::backward: //backward
            inp.mi.dwFlags = MOUSEEVENTF_XUP;
            inp.mi.mouseData = XBUTTON1;
            break;
    }
    SendInput(1, &inp, sizeof(inp));
}

void MouseHandler::holdKey(MouseHandler::Button key, int delay)
{
    pushKey(key);
    std::this_thread::sleep_for(delay);
    releaseKey(key);
}
