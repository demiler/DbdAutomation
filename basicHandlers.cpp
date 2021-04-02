#include "windows.h"
#include <array>
#include <functional>

template <class HOOK_TYPE, unsigned MAX_SIZE>
class BasicEventHandler {
protected:
    std::array<std::function<unsigned(bool)>, MAX_SIZE> callbacks;
    HOOK_TYPE hook;

    //some_type hookCallback(some args) = 0; should be in any dir classes
public:
    //calls SetWindowHook or smth like that, return true on success, othrewise false
    virtual bool init() = 0;

    //puts foo in map and returns the success of it
    virtual bool on(unsigned key, std::function<unsigned(bool)> foo, bool replace = false)
    {
        if (replace || !callbacks[key])
            callbacks[key] = foo;
        else
            return false;
        return true;
    }
};


template <unsigned MAX_KEYS = 0xFD>
class BasicDeviceHandler : public BasicEventHandler<HHOOK, MAX_KEYS> {
protected:
    std::array<bool, MAX_KEYS> buttonsState;

    //do all the work
    virtual LRESULT CALLBACK hookCallback(int, WPARAM, LPARAM) = 0;

public:
    enum State : unsigned { up = MAX_KEYS + 0b01, down = MAX_KEYS + 0b10 };
    enum Buttons : unsigned;

    virtual bool stateOf(unsigned key)
    {
        return buttonsState[key];
    }
};
