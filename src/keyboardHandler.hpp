#pragma once
#include <Windows.h>
#include <list>
#include <map>
#include <iostream>
#include "./hookSubscriber.hpp"
#include "./commonEnums.hpp"
#include "./exceptions.hpp"

class Keyboard {
public:
    enum class Key;

    Keyboard() {
        inp[0] = inp[1] = { 0 };
        inp[0].type = inp[1].type = INPUT_KEYBOARD;
        inp[1].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    State operator[] (Key key) {
        //SHORT winState = GetKeyState(KeyToVkCode(key));
        SHORT winState = GetAsyncKeyState(KeyToVkCode(key));
        //if (winState == NULL)
        //    throw winapiError("Failed to get key state");
        return HIBYTE(winState) ? State::down : State::up;
    }

    void push(Key key) {
        inp[0].ki.wVk = KeyToVkCode(key);
        if (SendInput(1, &inp[0], sizeof(*inp)) != 1)
            throw winapiError("Failed to correctly send keyboard push");
    }

    void release(Key key) {
        inp[1].ki.wVk = KeyToVkCode(key);
        if (SendInput(1, &inp[1], sizeof(*inp)) != 1)
            throw winapiError("Failed to correctly send keyboard release");
    }

    void press(Key key, int delay) {
        if (delay > 0) {
            push(key);
            Sleep(delay);
            release(key);
        }
        else {
            inp[0].ki.wVk = inp[1].ki.wVk = KeyToVkCode(key);
            if (SendInput(2, inp, sizeof(*inp)) != 2)
                throw winapiError("Failed to correctly send keyboard press");
        }
    }

private:
    static WORD KeyToVkCode(Key key) {
        return static_cast<WORD>(key);
    }
    INPUT inp[2];
};

enum class Keyboard::Key {
    unknown = 0x0,
    backspace = 0x08,
    tab = 0x09,
    clear = 0x0C,
    enter = 0x0D,
    //shift = 0x10,
    //ctrl = 0x11,
    //alt = 0x12,
    shift = 0xA0,
    ctrl = 0xA2,
    alt = 0xA4,
    pause = 0x13,
    caps = 0x14,
    esc = 0x1B,
    space = 0x20,
    pgup = 0x21,
    pgdown = 0x22,
    end = 0x23,
    home = 0x24,
    left = 0x25,
    up = 0x26,
    right = 0x27,
    down = 0x28,
    sel = 0x29,
    print = 0x2A,
    execute = 0x2B,
    ptrscr = 0x2C,
    ins = 0x2D,
    del = 0x2E,
    help = 0x2F,
    n0 = 0x30,
    n1 = 0x31,
    n2 = 0x32,
    n3 = 0x33,
    n4 = 0x34,
    n5 = 0x35,
    n6 = 0x36,
    n7 = 0x37,
    n8 = 0x38,
    n9 = 0x39,
    a = 0x41,
    b = 0x42,
    c = 0x43,
    d = 0x44,
    e = 0x45,
    f = 0x46,
    g = 0x47,
    h = 0x48,
    i = 0x49,
    j = 0x4A,
    k = 0x4B,
    l = 0x4C,
    m = 0x4D,
    n = 0x4E,
    o = 0x4F,
    p = 0x50,
    q = 0x51,
    r = 0x52,
    s = 0x53,
    t = 0x54,
    u = 0x55,
    v = 0x56,
    w = 0x57,
    x = 0x58,
    y = 0x59,
    z = 0x5A,
    num0 = 0x60,
    num1 = 0x61,
    num2 = 0x62,
    num3 = 0x63,
    num4 = 0x64,
    num5 = 0x65,
    num6 = 0x66,
    num7 = 0x67,
    num8 = 0x68,
    num9 = 0x69,
    nummul = 0x6A,
    numadd = 0x6B,
    separator = 0x6C,
    subtract = 0x6D,
    decimal = 0x6E,
    divide = 0x6F,
    f1 = 0x70,
    f2 = 0x71,
    f3 = 0x72,
    f4 = 0x73,
    f5 = 0x74,
    f6 = 0x75,
    f7 = 0x76,
    f8 = 0x77,
    f9 = 0x78,
    f10 = 0x79,
    f11 = 0x7A,
    f12 = 0x7B,
    f13 = 0x7C,
    f14 = 0x7D,
    f15 = 0x7E,
    f16 = 0x7F,
    f17 = 0x80,
    f18 = 0x81,
    f19 = 0x82,
    f20 = 0x83,
    f21 = 0x84,
    f22 = 0x85,
    f23 = 0x86,
    f24 = 0x87,
    num = 0x90,
    lshift = 0xA0,
    rshift = 0xA1,
    lctrl = 0xA2,
    rctrl = 0xA3,
    lalt = 0xA4,
    ralt = 0xA5,
    lwin = 0x5B,
    rwin = 0x5C,
};