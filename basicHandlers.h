#pragma once
#include "windows.h"
#include <map>
#include <functional>
#include "utils.h"

template <class HOOK_TYPE>
class BasicEventHandler {
protected:
    std::map<unsigned, std::function<unsigned(bool)>> callbacks;
    HOOK_TYPE hook;

    //some_type hookCallback(some args) = 0; should be in any dir classes
public:

    //puts function in array of callbacks and returns true on success
    virtual bool on(unsigned, std::function<unsigned(bool)>, bool replace = false);
};

template <unsigned MAX_KEYS = 0xFF>
class BasicDeviceHandler : public BasicEventHandler<HHOOK> {
protected:
    std::array<bool, MAX_KEYS> buttonsState;
    std::array<bool, MAX_KEYS> lockUpdate;
    bool ignoreInjections;

    //does all the work
    virtual LRESULT CALLBACK hookCallback(int, WPARAM, LPARAM) = 0;

    bool isUpState(unsigned state);
    bool isDownState(unsigned state);
    bool getKeyOnly(unsigned key);

    INPUT inp;

public:
    enum State : unsigned {
        up = highestBit(MAX_KEYS) << 1u,
        down = highestBit(MAX_KEYS) << 2u,
        any = up | down;
    };
    enum Button : unsigned;

    BasicEventHandler(bool ignoreInjections = true);

    virtual bool stateOf(unsigned key);
    virtual void setButtonUpdate(unsigned, bool);
    virtual bool getButtonUpdateState(unsigned key);
    virtual bool on(unsigned, std::function<unsigned(bool)>, bool replace = false);
    virtual bool callIfPresent(unsigned);

    virtual void pushKey(Button key) = 0;
    virtual void releaseKey(Button key) = 0;
    virtual void holdKey(Button key, int delay) = 0;
};
