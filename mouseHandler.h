#include "basicHandlers.h"

class MouseHandler : BasicDeviceHandler<0xF> {
    bool skipMouseMovment;

    unsigned fromVirtualCode(WPARAM, MSLLHOOKSTRUCT);
    LRESULT CALLBACK hookCallback(int, WPARAM, LPARAM) final;

public:
    enum Button : unsigned {
        unknown, movment, left, middle, right, forward, backward;
    };

    MouseHandler(bool skipMouseMovment = true, bool ignoreInjections = true);
};
