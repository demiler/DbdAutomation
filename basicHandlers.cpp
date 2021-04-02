#include "basicHandlers.h"

template <class HOOK_TYPE>
bool BasicEventHandler<HOOK_TYPE>::on(
        unsigned key, std::function<unsigned(bool)> foo, bool replace)
{
    if (replace || callbacks.find(key) == callbacks.end())
        callbacks[key] = foo;
    else
        return false;
    return true;
}

template <unsigned MAX_KEYS>
bool BasicDeviceHandler<MAX_KEYS>::isUpState(unsigned state)
{
    return state & BasicDeviceHandler<MAX_KEYS>::State::up;
}

template <unsigned MAX_KEYS>
bool BasicDeviceHandler<MAX_KEYS>::isDownState(unsigned state)
{
    return state & BasicDeviceHandler<MAX_KEYS>::State::down;
}

template <unsigned MAX_KEYS>
bool BasicDeviceHandler<MAX_KEYS>::getKeyOnly(unsigned key)
{
    return key & (~BasicDeviceHandler<MAX_KEYS>::State::any);
}

template <unsigned MAX_KEYS>
bool BasicDeviceHandler<MAX_KEYS>::on(
        unsigned key, std::function<unsigned(bool)> foo, bool replace)
{
    if (!isUpState(key) && !isDownState(key))
        key |= BasicDeviceHandler<MAX_KEYS>::State::any;
    BasicEventHandler<HHOOK>::on(key, foo, replace);
}

template <unsigned MAX_KEYS>
bool BasicDeviceHandler<MAX_KEYS>::stateOf(unsigned key)
{
    return buttonsState[getKeyOnly(key)];
}

template <unsigned MAX_KEYS>
void BasicDeviceHandler<MAX_KEYS>::setButtonUpdate(unsigned key, bool update)
{
    lockUpdate[getKeyOnly(key)] = update;
}

template <unsigned MAX_KEYS>
bool BasicDeviceHandler<MAX_KEYS>::getButtonUpdateState(unsigned key)
{
    return lockUpdate[getKeyOnly(key)];
}

template <unsigned MAX_KEYS>
BasicEventHandler::BasicEventHandler(bool ignoreInjections) :
    ignoreInjections(ignoreInjections) {}

template <unsigned MAX_KEYS>
bool BasicDeviceHandler<MAX_KEYS>::callIfPresent(unsigned key) {
    auto foo = callbacks.find(key);
    if (foo == callbacks.end()) {
        foo = callbacks.find(getKeyOnly(key));
        if (foo != callbacks.end())
            foo(isUpState(key));
    }
    else
        foo(isUpState(key));
}
