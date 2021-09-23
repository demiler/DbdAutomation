#pragma once
#include <map>
#include <string>
#include <future>
#include "./commonEnums.hpp"
#include "./mouseHandler.hpp"
#include "./keyboardHandler.hpp"
#include "./focusSubscriber.hpp"

using Key = Keyboard::Key;
using Button = Mouse::Button;

class EventHandler {
public:
    enum class Events {
        unset,
        app_focused, app_blured, script_restart,
        script_action, script_stop, script_start,
        exit,
    };

    struct Event {
        Events type;
        unsigned value;

        Event();
        Event(const Event&);
        Event(Events, unsigned val = 0);
    };

    EventHandler(bool raised = true);
    ~EventHandler();

    void raise();
    void fire(const Event&);

    const std::string& getEventName();
    unsigned getValue();
    operator Events();

    static void initHooks();

    void onMouseUp(Button btn, Event event, Flags flags = Flags::null);
    void onMouseDown(Button btn, Event event, Flags flags = Flags::null);

    void onKeyUp(Key btn, Event event, Flags flags = Flags::null);
    void onKeyDown(Key btn, Event event, Flags flags = Flags::null);

    void onBlur(Events evType);
    void onFocus(Events evType);

    void watchAppFocus(const char* path);

private:
    enum Focus { focus, blur, unset };

    typedef std::list<std::pair<Event, Flags>> triggerList_t;
    typedef std::pair<Button, State> msSearchKey_t;
    typedef std::pair<Key, State> kbSearchKey_t;
    typedef HookSubscriber<WH_MOUSE_LL> MsHkSub;
    typedef HookSubscriber<WH_KEYBOARD_LL> KbHkSub;

    template <class btnSearch_t, class Button_t>
    void addTrigger(std::map<btnSearch_t, triggerList_t>& triggerMap, State state, Button_t btn, Event event, Flags flags);

    template <DWORD INJECTED, class btnSearch_t, class Button_t, class Data_t>
    void handleFires(std::map<btnSearch_t, triggerList_t> triggerMap, Button_t btn, State state, Data_t data);

    bool mouseCallback(WPARAM action, LPARAM lParam);
    bool keyboardCallback(WPARAM action, LPARAM lp);
    void focusCallback(const char*);

    EventHandler(const EventHandler&) = delete;
    EventHandler(EventHandler&&) = delete;

    std::map<msSearchKey_t, triggerList_t> msTriggers;
    std::map<kbSearchKey_t, triggerList_t> kbTriggers;
    std::list<Events> fcTriggers[2];
    MsHkSub::subID_t msSubID;
    KbHkSub::subID_t kbSubID;
    FocusSubscriber::subID_t fcSubID;

    Focus lastFocusEvent;
    const char *applicationPath;

    Event current;
    std::promise<void> sync_prom;
    std::future<void> sync;
    bool raised, fired;
    static std::map<Events, std::string> eventsNames;
};
