#include "basicHandlers.h"

class KeyboardHandler : public BasicDeviceHandler<0xFF> {
    LRESULT CALLBACK hookCallback(int, WPARAM, LPARAM) final;
public:
    KeyboardHandler(bool ignoreInjections = true);
    ~KeyboardHandler();
};
