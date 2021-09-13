#pragma once
#include <Windows.h>
#include <functional>
#include <cstring>
#include <psapi.h>
#include <list>

class hookEvent_t {
	HWINEVENTHOOK _hook;
public:
	hookEvent_t() : _hook(NULL) {}
	hookEvent_t(WINEVENTPROC cb) { hook(cb); }
	~hookEvent_t() { UnhookWinEvent(_hook); }
	bool hooked() { return _hook != NULL; }
	void operator=(hookEvent_t&& old) noexcept { std::swap(_hook, old._hook); }

	void hook(WINEVENTPROC cb) {
		_hook = SetWinEventHook(
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
	static subID_t subscribe(void(T::* foo)(void), T* inst) {
		auto callback = std::bind(foo, inst);
		return subscribe(callback);
	}

	static void unsubscribe(subID_t subID) {
		subs.erase(subID);
	}

	static void init() {
		if (hook.hooked()) return;
		hook.hook(reinterpret_cast<WINEVENTPROC>(SubInvoker));
	}

private:
	//ignore all other params, cdecl will clear stack for us
	static void SubInvoker(HWINEVENTHOOK hWinEventHook, DWORD _, HWND hwnd) {
		if (hwnd == NULL) return;

		DWORD pid;
		GetWindowThreadProcessId(hwnd, &pid);
		HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

		char processPath[MAX_PATH];
		if (processHandle != NULL && GetModuleFileNameEx(processHandle, NULL, processPath, MAX_PATH)) {
			for (const auto& sub : subs) sub(processPath);
		}
	}

	static subList_t subs;
	static hookEvent_t hook;
};

FocusSubscriber::subList_t FocusSubscriber::subs;
hookEvent_t FocusSubscriber::hook;