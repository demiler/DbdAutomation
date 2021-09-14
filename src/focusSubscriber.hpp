#pragma once
#include <Windows.h>
#include <functional>
#include <cstring>
#include <psapi.h>
#include <list>
#include <iostream>

class hookEvent_t {
    HWINEVENTHOOK hook;
public:
    hookEvent_t() : hook(NULL) {}
    hookEvent_t(WINEVENTPROC cb) { init(cb); }
    ~hookEvent_t() { UnhookWinEvent(hook); }
    bool hooked() { return hook != NULL; }
    void operator=(hookEvent_t&& old) noexcept { std::swap(hook, old.hook); }

    void init(WINEVENTPROC cb) {
        hook = SetWinEventHook(
            EVENT_OBJECT_FOCUS, EVENT_OBJECT_FOCUS,
            NULL, cb, 0, 0,
            WINEVENT_OUTOFCONTEXT
        );
    }
};

class FocusSubscriber {
public:
    typedef std::function<void(const char*)> callback_t;
    typedef std::list<callback_t> subList_t;
    typedef subList_t::iterator subID_t;

    static subID_t subscribe(callback_t subcb) {
        return subs.insert(std::end(subs), subcb);
    }

    template <class T>
    static subID_t subscribe(void(T::* foo)(const char*), T* inst) {
        using namespace std::placeholders;
        auto callback = std::bind(foo, inst, _1);
        return subscribe(callback);
    }

    static void unsubscribe(subID_t subID) {
        subs.erase(subID);
    }

    static void init() {
        if (hook.hooked()) return;
        hook.init(reinterpret_cast<WINEVENTPROC>(SubInvoker));
    }

    static const char* currentFocus() {
        static char processPath[MAX_PATH];
        
        DWORD pid;
        HWND curWindow = GetForegroundWindow();
        GetWindowThreadProcessId(curWindow, &pid);
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

        if (processHandle != NULL) {
            if (GetModuleFileNameEx(processHandle, NULL, processPath, MAX_PATH)) {
                for (const auto& sub : subs) sub(processPath);
            }
            CloseHandle(processHandle);
        }
        return processPath;
    }

private:
    //ignore all other params, cdecl will clear stack for us
    static void SubInvoker(HWINEVENTHOOK hWinEventHook, DWORD _, HWND hwnd) {
        if (hwnd == NULL) return;

        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

        char processPath[MAX_PATH];
        if (processHandle != NULL) {
            if (GetModuleFileNameEx(processHandle, NULL, processPath, MAX_PATH)) {
                for (const auto& sub : subs) sub(processPath);
            }
            CloseHandle(processHandle);
        }
    }

    static subList_t subs;
    static hookEvent_t hook;
};