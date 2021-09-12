#pragma once
#include <Windows.h>
#include <functional>
#include <list>
#include <utility>

class hook_t {
	HHOOK hook;
public:
	hook_t() : hook(NULL) {}
	hook_t(int HOOK_ID, HOOKPROC cb) : hook(SetWindowsHookExA(HOOK_ID, cb, NULL, 0)) {}
	~hook_t() { if (hook != NULL) UnhookWindowsHookEx(hook); }
	bool hooked() { return hook != NULL; }
	void operator=(hook_t&& old) noexcept { std::swap(hook, old.hook); }
};

template <int HOOK_ID>
class HookSubscriber {
public:
	typedef std::function<void(WPARAM, LPARAM)> callback_t;
	typedef std::list<callback_t>::iterator subID_t;

	static subID_t subscribe(callback_t sub) {
		return HookSubscriber<HOOK_ID>::subs.insert(std::end(subs), sub);
	}

	template <class T>
	static subID_t subscribe(void(T::*foo)(WPARAM, LPARAM), T* inst) {
		using namespace std::placeholders;
		auto callback = std::bind(foo, inst, _1, _2);
		return  HookSubscriber<HOOK_ID>::subscribe(callback);
	}

	static void unsubscribe(subID_t subID) {
		HookSubscriber<HOOK_ID>::subs.erase(subID);
	}

	static void init() {
		if (HookSubscriber<HOOK_ID>::hook.hooked()) return;
		HookSubscriber<HOOK_ID>::hook = { HOOK_ID, HookSubscriber<HOOK_ID>::SubInvoker };
	}

private:
	static std::list<callback_t> subs;
	static hook_t hook;

	static LRESULT CALLBACK SubInvoker(int nCode, WPARAM wp, LPARAM lp) {
		if (nCode >= 0) {
			for (const auto& sub : HookSubscriber<HOOK_ID>::subs)
				sub(wp, lp);
		}
		return CallNextHookEx(NULL, nCode, wp, lp);
	}
};

template <int HOOK_ID>
hook_t HookSubscriber<HOOK_ID>::hook;

template <int HOOK_ID>
std::list<std::function<void(WPARAM, LPARAM)>> HookSubscriber<HOOK_ID>::subs;