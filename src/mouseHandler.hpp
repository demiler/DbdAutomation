#pragma once
#include <Windows.h>
#include <list>
#include <map>
#include <iostream>
#include "./hookSubscriber.hpp"
#include "./commonEnums.hpp"
#include "./exceptions.hpp"

class Mouse {
public:
    enum class Button { left, middle, right, forward, backward, unknown };
    enum class mouseEvent { unknown, move, button, wheel };

    Mouse() {
        inp[0] = inp[1] = { 0 };
        inp[0].type = inp[1].type = INPUT_MOUSE;
        subID = MsHookSub::subscribe(&Mouse::updateButtons, this);

        for (size_t i = 0; i < MOUSE_BUTTONS_COUNT; ++i) {
            buttonMap[i] = this->getWinState(Button(i));
        }
    }

    ~Mouse() {
        MsHookSub::unsubscribe(subID);
    }

    State getWinState(Button btn) {
        SHORT winState = GetKeyState(BtnToVkCode(btn));
        //if (winState == NULL)
        //    throw winapiError("Failed to get mouse button state");
        return HIBYTE(winState) ? State::down : State::up;
    }

    State operator[] (Button btn) {
        return buttonMap[static_cast<int>(btn)];
    }

    void push(Button btn) {
        inp[0].mi.dwFlags = BtnToFlags(btn, State::down);
        inp[0].mi.mouseData = BtnToData(btn);
        if (SendInput(1, &inp[0], sizeof(*inp)) != 1)
            throw winapiError("Falied to correctly send mouse push");
    }

    void release(Button btn) {
        inp[1].mi.dwFlags = BtnToFlags(btn, State::up);
        inp[1].mi.mouseData = BtnToData(btn);
        if (SendInput(1, &inp[1], sizeof(*inp)) != 1)
            throw winapiError("Falied to correctly send mouse release");
    }

    void press(Button btn, int delay) {
        if (delay > 0) {
            push(btn);
            Sleep(delay);
            release(btn);
        }
        else {
            inp[0].mi.dwFlags = BtnToFlags(btn, State::down);
            inp[1].mi.dwFlags = BtnToFlags(btn, State::up);
            inp[0].mi.mouseData = inp[1].mi.mouseData = BtnToData(btn);
            if (SendInput(2, inp, sizeof(*inp)) != 2)
                throw winapiError("Falied to correctly send mouse press");
        }
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
    void updateButtons(WPARAM wp, LPARAM lp) {
        auto data = *reinterpret_cast<MSLLHOOKSTRUCT*>(lp);

        if (data.flags != LLMHF_INJECTED) {
            auto btnPair = Mouse::identifyButton(wp, data);
            if (btnPair.first != Button::unknown) {
                buttonMap[static_cast<size_t>(btnPair.first)] = btnPair.second;
            }
        }
    }

    WORD BtnToVkCode(Button button) {
        switch (button) {
            case Button::left:     return VK_LBUTTON;
            case Button::middle:   return VK_MBUTTON;
            case Button::right:    return VK_RBUTTON;
            case Button::backward: return VK_XBUTTON1;
            case Button::forward:  return VK_XBUTTON2;
            default:
                return NULL;
        }
    }

    WORD BtnToFlags(Button button, State state) {
        if (state == State::up) {
            switch (button) {
                case Button::left:    return MOUSEEVENTF_LEFTUP;
                case Button::middle:  return MOUSEEVENTF_MIDDLEUP;
                case Button::right:   return MOUSEEVENTF_RIGHTUP;
                case Button::backward:
                case Button::forward: return MOUSEEVENTF_XUP;
            }
        }
        else {
            switch (button) {
                case Button::left:     return MOUSEEVENTF_LEFTDOWN;
                case Button::middle:   return MOUSEEVENTF_MIDDLEDOWN;
                case Button::right:    return MOUSEEVENTF_RIGHTDOWN;
                case Button::backward:
                case Button::forward:  return MOUSEEVENTF_XDOWN;
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
    static const size_t MOUSE_BUTTONS_COUNT = 5;
    typedef HookSubscriber<WH_MOUSE_LL> MsHookSub;

    INPUT inp[2];
    MsHookSub::subID_t subID;
    State buttonMap[MOUSE_BUTTONS_COUNT];
};