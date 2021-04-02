#include "keyboard.h"

LRESULT CALLBACK KeyboardHandler::hookCallback(int nCode, WPARAM action, LPARAM rawData)
{
    if (nCode < 0)
        return CallNextHookEx(NULL, nCode, action, rawData);

    KBDLLHOOKSTRUCT data = *(static_cast<KBDLLHOOKSTRUCT*>(rawData));

    if (data.flags == LLKHF_INJECTED && ignoreInjections)
        return CallNextHookEx(NULL, nCode, action, rawData);

    Keyboard::Button actionCode = data.vkCode;
    if (action == WM_KEYUP || action == WM_SYSKEYUP)
        actionCode |= Keyboard::State::up;
    else if (action == WM_KEYDOWN || action == WM_KEYDOWN)
        actionCode |= Keyboard::State::down;

    auto key = getKeyOnly(actionCode);
    buttonsState[key] = isUpState(actionCode);

    //by winapi documentation to lock button update from hook, you need to return non-zero
    return lockUpdate[key] ? 1 : CallNextHookEx(NULL, nCode, action, rawData);
}

KeyboardHandler::KeyboardHandler(bool ignoreInjections) :
    BasicDeviceHandler::BasicDeviceHandler(ignoreInjections)
{
    hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHandler::hookCallback, NULL, 0);
    if (!hook)
        throw "unable to create mouse hook";
}

KeyboardHandler::~KeyboardHandler()
{
    UnhookWindowsHookEx(hook);
}

KeyboardHandler::Button KeyboardHandler::fromString(const std::string& name)
{
    return keyNames[name];
}

std::map<std::string, KeyboardHandler::Button> KeyboardHandler::keyNames = {
    { "backspace", KeyboardHandler::Button::backspace },
    { "tab",       KeyboardHandler::Button::tab       },
    { "clear",     KeyboardHandler::Button::clear     },
    { "enter",     KeyboardHandler::Button::enter     },
    { "shift",     KeyboardHandler::Button::shift     },
    { "ctrl",      KeyboardHandler::Button::ctrl      },
    { "alt",       KeyboardHandler::Button::alt       },
    { "shift",     KeyboardHandler::Button::shift     },
    { "ctrl",      KeyboardHandler::Button::ctrl      },
    { "alt",       KeyboardHandler::Button::alt       },
    { "pause",     KeyboardHandler::Button::pause     },
    { "caps",      KeyboardHandler::Button::caps      },
    { "esc",       KeyboardHandler::Button::esc       },
    { "space",     KeyboardHandler::Button::space     },
    { "pgup",      KeyboardHandler::Button::pgup      },
    { "pgdown",    KeyboardHandler::Button::pgdown    },
    { "end",       KeyboardHandler::Button::end       },
    { "home",      KeyboardHandler::Button::home      },
    { "left",      KeyboardHandler::Button::left      },
    { "up",        KeyboardHandler::Button::up        },
    { "right",     KeyboardHandler::Button::right     },
    { "down",      KeyboardHandler::Button::down      },
    { "sel",       KeyboardHandler::Button::sel       },
    { "print",     KeyboardHandler::Button::print     },
    { "execute",   KeyboardHandler::Button::execute   },
    { "ptrscr",    KeyboardHandler::Button::ptrscr    },
    { "ins",       KeyboardHandler::Button::ins       },
    { "del",       KeyboardHandler::Button::del       },
    { "help",      KeyboardHandler::Button::help      },
    { "0",         KeyboardHandler::Button::n0        },
    { "1",         KeyboardHandler::Button::n1        },
    { "2",         KeyboardHandler::Button::n2        },
    { "3",         KeyboardHandler::Button::n3        },
    { "4",         KeyboardHandler::Button::n4        },
    { "5",         KeyboardHandler::Button::n5        },
    { "6",         KeyboardHandler::Button::n6        },
    { "7",         KeyboardHandler::Button::n7        },
    { "8",         KeyboardHandler::Button::n8        },
    { "9",         KeyboardHandler::Button::n9        },
    { "a",         KeyboardHandler::Button::a         },
    { "b",         KeyboardHandler::Button::b         },
    { "c",         KeyboardHandler::Button::c         },
    { "d",         KeyboardHandler::Button::d         },
    { "e",         KeyboardHandler::Button::e         },
    { "f",         KeyboardHandler::Button::f         },
    { "g",         KeyboardHandler::Button::g         },
    { "h",         KeyboardHandler::Button::h         },
    { "i",         KeyboardHandler::Button::i         },
    { "j",         KeyboardHandler::Button::j         },
    { "k",         KeyboardHandler::Button::k         },
    { "l",         KeyboardHandler::Button::l         },
    { "m",         KeyboardHandler::Button::m         },
    { "n",         KeyboardHandler::Button::n         },
    { "o",         KeyboardHandler::Button::o         },
    { "p",         KeyboardHandler::Button::p         },
    { "q",         KeyboardHandler::Button::q         },
    { "r",         KeyboardHandler::Button::r         },
    { "s",         KeyboardHandler::Button::s         },
    { "t",         KeyboardHandler::Button::t         },
    { "u",         KeyboardHandler::Button::u         },
    { "v",         KeyboardHandler::Button::v         },
    { "w",         KeyboardHandler::Button::w         },
    { "x",         KeyboardHandler::Button::x         },
    { "y",         KeyboardHandler::Button::y         },
    { "z",         KeyboardHandler::Button::z         },
    { "num0",      KeyboardHandler::Button::num0      },
    { "num1",      KeyboardHandler::Button::num1      },
    { "num2",      KeyboardHandler::Button::num2      },
    { "num3",      KeyboardHandler::Button::num3      },
    { "num4",      KeyboardHandler::Button::num4      },
    { "num5",      KeyboardHandler::Button::num5      },
    { "num6",      KeyboardHandler::Button::num6      },
    { "num7",      KeyboardHandler::Button::num7      },
    { "num8",      KeyboardHandler::Button::num8      },
    { "num9",      KeyboardHandler::Button::num9      },
    { "nummul",    KeyboardHandler::Button::nummul    },
    { "numadd",    KeyboardHandler::Button::numadd    },
    { "separator", KeyboardHandler::Button::separator },
    { "subtract",  KeyboardHandler::Button::subtract  },
    { "decimal",   KeyboardHandler::Button::decimal   },
    { "divide",    KeyboardHandler::Button::divide    },
    { "f1",        KeyboardHandler::Button::f1        },
    { "f2",        KeyboardHandler::Button::f2        },
    { "f3",        KeyboardHandler::Button::f3        },
    { "f4",        KeyboardHandler::Button::f4        },
    { "f5",        KeyboardHandler::Button::f5        },
    { "f6",        KeyboardHandler::Button::f6        },
    { "f7",        KeyboardHandler::Button::f7        },
    { "f8",        KeyboardHandler::Button::f8        },
    { "f9",        KeyboardHandler::Button::f9        },
    { "f10",       KeyboardHandler::Button::f10       },
    { "f11",       KeyboardHandler::Button::f11       },
    { "f12",       KeyboardHandler::Button::f12       },
    { "f13",       KeyboardHandler::Button::f13       },
    { "f14",       KeyboardHandler::Button::f14       },
    { "f15",       KeyboardHandler::Button::f15       },
    { "f16",       KeyboardHandler::Button::f16       },
    { "f17",       KeyboardHandler::Button::f17       },
    { "f18",       KeyboardHandler::Button::f18       },
    { "f19",       KeyboardHandler::Button::f19       },
    { "f20",       KeyboardHandler::Button::f20       },
    { "f21",       KeyboardHandler::Button::f21       },
    { "f22",       KeyboardHandler::Button::f22       },
    { "f23",       KeyboardHandler::Button::f23       },
    { "f24",       KeyboardHandler::Button::f24       },
    { "num",       KeyboardHandler::Button::num       },
    { "lshift",    KeyboardHandler::Button::lshift    },
    { "rshift",    KeyboardHandler::Button::rshift    },
    { "lctrl",     KeyboardHandler::Button::lctrl     },
    { "rctrl",     KeyboardHandler::Button::rctrl     },
    { "lalt",      KeyboardHandler::Button::lalt      },
    { "ralt",      KeyboardHandler::Button::ralt      },
    { "lwin",      KeyboardHandler::Button::lwin      },
    { "rwin",      KeyboardHandler::Button::rwin      },
};
