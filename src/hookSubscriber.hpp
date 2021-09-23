#pragma once
#include <Windows.h>
#include <functional>
#include <list>
#include <utility>
#include <spdlog/spdlog.h>

class hook_t {
    HHOOK hook;
public:
    hook_t() : hook(NULL) {}
    hook_t(int HOOK_ID, HOOKPROC cb) {
        hook = SetWindowsHookExA(HOOK_ID, cb, NULL, 0);
        if (hook == NULL) {
            spdlog::error("Unable to set hook id: {}", HOOK_ID);
        }
    }
    ~hook_t() { if (hooked()) UnhookWindowsHookEx(hook); }
    bool hooked() { return hook != NULL; }
    void operator=(hook_t&& old) noexcept { std::swap(hook, old.hook); }
};

template <int HOOK_ID>
class HookSubscriber {
public:
    typedef std::function<bool(WPARAM, LPARAM)> callback_t;
    typedef std::list<callback_t>::iterator subID_t;

    static subID_t subscribe(callback_t sub) {
        return subs.insert(std::end(subs), sub);
    }

    template <class T>
    static subID_t subscribe(bool(T::*foo)(WPARAM, LPARAM), T* inst) {
        using namespace std::placeholders;
        auto callback = std::bind(foo, inst, _1, _2);
        return subscribe(callback);
    }

    static void unsubscribe(subID_t subID) {
        subs.erase(subID);
    }

    static void init() {
        if (hook.hooked()) return;
        hook = { HOOK_ID, SubInvoker };
    }

private:
    static std::list<callback_t> subs;
    static hook_t hook;

    static LRESULT CALLBACK SubInvoker(int nCode, WPARAM wp, LPARAM lp) {
        if (nCode >= 0) {
            bool blockEvent = false;
            for (const auto& sub : subs) {
                if (sub(wp, lp)) blockEvent = true;
            }
            //return any value > 0 to block input
            if (blockEvent) return 1;
        }
        return CallNextHookEx(NULL, nCode, wp, lp);
    }
};

template <int HOOK_ID>
hook_t HookSubscriber<HOOK_ID>::hook;

template <int HOOK_ID>
std::list<std::function<bool(WPARAM, LPARAM)>> HookSubscriber<HOOK_ID>::subs;