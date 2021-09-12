#pragma once
#include <Windows.h>
#include <list>
#include <map>
#include "./hookSubscriber.hpp"
#include "./commonEnums.hpp"

class Mouse {
public:
    enum class Button { unknown, left, middle, right, forward, backward };
    enum class mouseEvent { unknown, move, button, wheel };

    Mouse() {
        inp[0] = inp[1] = { 0 };
        inp[0].type = inp[1].type = INPUT_MOUSE;

        subID = MsSubEv::subscribe(&Mouse::updateBtnState, this);
        for (size_t i = 0; i < BUTTON_MAP_SIZE; ++i) buttonMap[i] = State::up;
    }

    ~Mouse() {
        MsSubEv::unsubscribe(subID);
    }

    State operator[] (Button key) {
        return buttonMap[static_cast<int>(key)];
    }

    void push(Button btn) {
        inp->mi.dwFlags = BtnToFlags(btn, State::down);
        inp->mi.mouseData = BtnToData(btn);
        SendInput(1, inp, sizeof(*inp));
    }

    void release(Button btn) {
        inp->mi.dwFlags = BtnToFlags(btn, State::up);
        inp->mi.mouseData = BtnToData(btn);
        SendInput(1, inp + 1, sizeof(*inp));
    }

    void press(Button btn, int delay) {
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

    static mouseEvent identifyEvent(WPARAM wParam) {
        switch (wParam) {
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_XBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_XBUTTONUP:
                return mouseEvent::button;

            case WM_MOUSEWHEEL:
                return mouseEvent::wheel;

            case WM_MOUSEMOVE:
                return mouseEvent::move;

            default:
                return mouseEvent::unknown;
        }
    }

    static std::pair<Button, State> identifyButton(WPARAM wParam, MSLLHOOKSTRUCT data) {
        State state;
        Button btn;

        switch (wParam) {
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
            default:
                btn = Button::unknown;
        }
        return std::make_pair(btn, state);
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

    void updateBtnState(WPARAM wp, LPARAM lp) {
        MSLLHOOKSTRUCT data = *reinterpret_cast<MSLLHOOKSTRUCT*>(lp);
        auto buttonPair = Mouse::identifyButton(wp, data);

        if (buttonPair.first != Button::unknown)
            buttonMap[static_cast<int>(buttonPair.first)] = buttonPair.second;
    }

    typedef HookSubscriber<WH_MOUSE_LL> MsSubEv;
    static const size_t BUTTON_MAP_SIZE = 32;

    State buttonMap[BUTTON_MAP_SIZE];
    MsSubEv::subID_t subID;
    INPUT inp[2];
};