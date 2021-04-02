#include "focusedHandler.h"

FocuseHandler::FocuseHandler(const TCHAR *AppName) {
    TCHAR ptr = this->AppName;
    while (this->AppName - ptr + 1 < MAX_LEN && *AppName)
        *(ptr++) = *(AppName++);
    *ptr = '\0';

    if (*AppName != '\0')
        log.warning("App name is to long!");

    hook = SetWinEventHook(
        EVENT_MIN,
        EVENT_MAX,
        GetModuleHandle(NULL),
        (WINEVENTPROC)FocuseHandler::hookCallback,
        0,
        0,
        WINEVENT_OUTOFCONTEXT);
    if (!hook)
        throw "Unable to create focus hook";
}

FocuseHandler::~FocuseHandler() {
    UnhookWinEvent(hook);
}

void FocuseHandler::callIfPresent(unsigned event)
{
    auto foo = callbacks.find(event);
    if (foo != callbacks.end())
        foo(focused)
}

void FocuseHandler::hookCallback(
  HWINEVENTHOOK hWinEventHook,
  DWORD event,
  HWND hwnd,
  LONG idObject,
  LONG idChild,
  DWORD idEventThread,
  DWORD dwmsEventTime)
{
    if (event != EVENT_OBJECT_FOCUS)
        return;

    if (hwnd == NULL) {
        //log.it("Focus event wasn't caused by window");
        return;
    }

    TCHAR title[MAX_LEN];
    int len = GetWindowTextA(hwnd, title, MAX_LEN);

    if (!len) {
        //log.it("Focused window has no title\n");
        if (focused) {
            focused = false;
            callIfPresent(FocuseHandler::Event::blurred);
        }
    }
    else {
        //log.it("Windows '%hs' focused", title);
        if (!_tcscmp(title, AppName)) {
            focused = true;
            callIfPresent(FocuseHandler::Event::focused);
        }
        else if (focused) {
            focused = false;
            callIfPresent(FocuseHandler::Event::blurred);
        }
    }
}

bool FocuseHandler::isFocused()
{
    return focused;
}
