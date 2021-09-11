#include <Windows.h>
#include <list>
#include <map>
#include <functional>
#include <utility>
#include "./eventHandler.hpp"
#include "./hookSubscriber.hpp"
#include "./commonEnums.hpp"

using Event = EventHandler::Event;

class MouseHandler {
public:
    enum class Button { left, middle, right, forward, backward };

    MouseHandler(EventHandler& eventer) : eventer(eventer) {
        inp[0] = inp[1] = { 0 };
        inp[0].type = inp[1].type = INPUT_MOUSE;
        inp[1].ki.dwFlags = KEYEVENTF_KEYUP;

		subID = MsSubEv::subscribe(&MouseHandler::callbackHandler, this);
        for (int i = 0; i < 32; i++) btnsMap[i] = State::up;
    }

    ~MouseHandler() {
        MsSubEv::unsubscribe(subID);
    }

    void onbtnup(Button btn, Event event, Flags flags = Flags::null) {
        addTrigger(State::up, btn, event, flags);
    }

    void onbtndown(Button btn, Event event, Flags flags = Flags::null) {
        addTrigger(State::down, btn, event, flags);
    }

    State operator[] (Button key) {
        return btnsMap[static_cast<int>(key)];
    }

    void press(Button btn) {
        inp->mi.dwFlags = BtnToFlags(btn, State::down);
        inp->mi.mouseData = BtnToData(btn);
        SendInput(1, inp, sizeof(*inp));
    }

    void release(Button btn) {
        inp->mi.dwFlags = BtnToFlags(btn, State::up);
        inp->mi.mouseData = BtnToData(btn);
        SendInput(1, inp + 1, sizeof(*inp));
    }

    void pressKey(Button btn, int delay) {
        inp[0].mi.dwFlags = BtnToFlags(btn, State::down);
        inp[1].mi.dwFlags = BtnToFlags(btn, State::up);
        inp[0].mi.mouseData = inp[1].mi.mouseData = BtnToData(btn);

        if (delay > 0) {
            SendInput(1, inp, sizeof(*inp));
            Sleep(delay);
            SendInput(1, inp + 1, sizeof(*inp));
        }
        else
            SendInput(2, inp, sizeof(*inp));
    }

private:
    WORD BtnToFlags(Button button, State state) {
        if (state == State::up) {
            switch (button) {
                case Button::left: return MOUSEEVENTF_LEFTUP;
                case Button::middle: return MOUSEEVENTF_MIDDLEUP;
                case Button::right: return MOUSEEVENTF_RIGHTUP;
                case Button::backward:
                case Button::forward: return MOUSEEVENTF_XUP;
            }
        }
        else {
            switch (button) {
            case Button::left: return MOUSEEVENTF_LEFTDOWN;
            case Button::middle: return MOUSEEVENTF_MIDDLEDOWN;
            case Button::right: return MOUSEEVENTF_RIGHTDOWN;
            case Button::backward:
            case Button::forward: return MOUSEEVENTF_XDOWN;
            }
        }
    }

    DWORD BtnToData(Button button) {
        switch (button) {
            case Button::forward: return XBUTTON2;
            case Button::backward: return XBUTTON1;
            default: return NULL;
        }
    }

    enum class msEvent { unknown, move, button, wheel };

    void updateBtnState(Button btn, State state) {
        btnsMap[static_cast<int>(btn)] = state;
    }

	void addTrigger(State state, Button key, Event event, Flags flags) {
		auto searchPair = std::make_pair(key, state);
		auto it = triggers.find(searchPair);

		if (it == triggers.end()) {
			triggerList_t list = { std::make_pair(event, flags) };
			triggers.emplace(searchPair, std::move(list));
		}
		else {
			it->second.emplace_back(std::make_pair(event, flags));
		}
	}

    void callbackHandler(WPARAM action, LPARAM lParam) {
        msEvent msev;
        switch (action) {
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_XBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_XBUTTONUP:
                msev = msEvent::button;
                break;

            case WM_MOUSEWHEEL:
                msev = msEvent::wheel;
                break;

            case WM_MOUSEMOVE:
                msev = msEvent::move;
                break;

            default:
                msev = msEvent::unknown;
        }

		if (msev != msEvent::button) return; //ignore all events except buttons press

        MSLLHOOKSTRUCT data = *((MSLLHOOKSTRUCT*)lParam);

        //if (data.flags == 0x1/*LLMHF_INJECTED*/)
        //return;

        Button btn;
        State state;
        switch (action) {
            case WM_LBUTTONDOWN: state = State::down; btn = Button::left; break;
            case WM_LBUTTONUP:   state = State::up;   btn = Button::left; break;

            case WM_RBUTTONDOWN: state = State::down; btn = Button::right; break;
            case WM_RBUTTONUP:   state = State::up;   btn = Button::right; break;

            case WM_MBUTTONDOWN: state = State::down; btn = Button::middle; break;
            case WM_MBUTTONUP:   state = State::up;   btn = Button::middle; break;

            case WM_XBUTTONDOWN:
                state = State::down;
                btn = (HIWORD(data.mouseData) == XBUTTON1)
                    ? Button::backward
                    : Button::forward;
                break;

            case WM_XBUTTONUP:
                state = State::up;
                btn = (HIWORD(data.mouseData) == XBUTTON1)
                    ? Button::backward
                    : Button::forward;
                break;
        }

        auto it = triggers.find(std::make_pair(btn, state));
		if (it != triggers.end()) {
            for (const auto& trigger : it->second) {
                Flags flags = trigger.second;
                if ((flags & Flags::notInjected) && (data.flags & LLKHF_INJECTED)) continue;
                //if ((flags & Flags::scriptActive) && isScriptRunning())      continue;

				EventHandler::Event a = trigger.first;
                eventer.fire(trigger.first);
            }
        }

        updateBtnState(btn, state);
    }

    typedef std::list<std::pair<Event, Flags>> triggerList_t;
    typedef std::pair<Button, State> searchKey_t;
	typedef HookSubscriber<WH_MOUSE_LL> MsSubEv;

    std::map<searchKey_t, triggerList_t> triggers;
    EventHandler &eventer;
    State btnsMap[32];
	MsSubEv::subID_t subID;
    INPUT inp[2];
};
