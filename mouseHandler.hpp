//#include <Windows.h>
#include "./eventHandler.hpp"
#include <list>
#include <map>
#include <functional>
#include <utility>

enum Flags { null = 0, notInjected = 0x1, scriptActive = 0x2 };

class MouseHandler {
public:
    enum class State { down, up };
    enum class Button { left, middle, right, forward, backward };

    MouseHandler(EventHandler& eventer) : eventer(eventer) {
        hook = SetWindowsHookEx(WH_KEYBOARD_LL, callbackHandler, NULL, 0);
        for (int i = 0; i < 32; i++) btnsMap[i] = State::up;
    }

    ~MouseHandler() {
        UnhookWindowsHookEx(hook);
    }

    void onbtnup(Button btn, Event event, Flags flags = Flags::null) {
        addTrigger(State::up, btn, event, flags);
    }

    void onbtndown(Button btn, Event event, Flags flags = Flags::null) {
        addTrigger(State::down, btn, event, flags);
    }

    State operator[] (Keys key) {
        return keysMap[static_cast<int>(key)];
    }

private:
    enum class msEvent { unknown, move, button, wheel };

    void updateBtnState(Button btn, State state) {
        btnsMap[static_cast<int>(btn)] = state;
    }

    void addTrigger(State state, Button btn, Event event, Flags flags) {
        auto it = triggers.find(std::make_pair(btn, state));
        if (it == triggers.end()) {
            triggerList_t list = { std::make_pair(event, flags) };
            triggers.emplace(btn, std::move(list));
        }
        else {
            it->second.emplace_back(std::make_pair(event, flags));
        }
    }

    LRESULT CALLBACK callbackHandler(int nCode, WPARAM action, LPARAM lParam) {
        if (nCode < 0) //not mouse msg
            return CallNextHookEx(NULL, nCode, action, lParam);

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

        if (msev != msEvent::button) //ignore all events except buttons press
            return CallNextHookEx(NULL, nCode, action, lParam);

        MSLLHOOKSTRUCT data = *((MSLLHOOKSTRUCT*)lParam);

        //if (data.flags == 0x1/*LLMHF_INJECTED*/)
        //return CallNextHookEx(NULL, nCode, action, lParam);

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
                if ((flags & Flags::notInjected) && isPressInjected(lParam)) continue;
                //if ((flags & Flags::scriptActive) && isScriptRunning())      continue;

                Event event = trigger.first;
                eventer.fire(event.type, event.value);
            }
        }

        updateBtnState(btn, state);

        return CallNextHookEx(NULL, nCode, action, lParam);
    }

    typedef std::list<std::pair<Event, Flags>> triggerList_t;
    typedef std::pair<Button, State> searchKey_t;

    std::map<searchKey_t, triggerList_t> triggers;
    EventHandler &eventer;
    State btnsMap[32];
    HHOOK hook;
};
