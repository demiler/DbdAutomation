#pragma once
#include <Windows.h>
#include <functional>
#include <cstring>
#include <psapi.h>
#include <list>
#include <iostream>
#include "./exceptions.hpp"
#include "./utils.hpp"
#include <spdlog/spdlog.h>

class hookEvent_t {
    HWINEVENTHOOK hook;
public:
    hookEvent_t() : hook(NULL) {}
    hookEvent_t(WINEVENTPROC cb) { init(cb); }
    ~hookEvent_t() { if (hooked()) UnhookWinEvent(hook); }
    bool hooked() { return hook != NULL; }
    void operator=(hookEvent_t&& old) noexcept { std::swap(hook, old.hook); }

    void init(WINEVENTPROC cb) {
        hook = SetWinEventHook(
            EVENT_OBJECT_FOCUS, EVENT_OBJECT_FOCUS,
            NULL, cb, 0, 0,
            WINEVENT_OUTOFCONTEXT
        );
        if (hook == NULL) {
            spdlog::error("Unable to set focus event hook");
            throw winapiError(GetLastError(), "Unable to set focus event hook");
        }
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
        
        HWND curWindow = GetForegroundWindow();
        if (curWindow == NULL) {
            spdlog::error("Unable to set focus event hook");
            throw winapiError("Unable to get foreground window");
        }

        getPathByHWND(curWindow, processPath);

        return processPath;
    }

private:
    //ignore all other params, cdecl will clear stack for us
    static void SubInvoker(HWINEVENTHOOK hWinEventHook, DWORD _, HWND hwnd) {
        if (hwnd == NULL) return;

        char processPath[MAX_PATH];
        getPathByHWND(hwnd, processPath);

        for (const auto& sub : subs) sub(processPath);
    }

    static subList_t subs;
    static hookEvent_t hook;
};