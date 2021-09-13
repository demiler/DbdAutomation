#include "../eventHandler.hpp"

/*  Structure implementation */
EventHandler::Event::
    Event() : type(Events::unset), value(0u) {}

EventHandler::Event::
    Event(const Event& ev) : type(ev.type), value(ev.value) {}

EventHandler::Event::
    Event(Events type, unsigned value) : type(type), value(value) {}

/* EventHandler implementation */
EventHandler::EventHandler(bool raise) : fired(false), applicationPath(nullptr) {
    if (raise) this->raise();
    else raised = false;

    msSubID = MsHkSub::subscribe(&EventHandler::mouseCallback, this);
    kbSubID = KbHkSub::subscribe(&EventHandler::keyboardCallback, this);
    fcSubID = FocusSubscriber::subscribe(&EventHandler::focusCallback, this);
}

EventHandler::~EventHandler() {
    MsHkSub::unsubscribe(msSubID);
    KbHkSub::unsubscribe(kbSubID);
    FocusSubscriber::unsubscribe(fcSubID);
}

void EventHandler::raise() {
    raised = true;
    sync_prom = std::promise<void>();
    sync = sync_prom.get_future();
}

void EventHandler::fire(const Event& event) {
    if (raised && !fired) {
        sync_prom.set_value();
        fired = true;
    }
    current = event;
}

unsigned EventHandler::getValue() {
    return current.value;
}

const std::string& EventHandler::getEventName() {
    return EventHandler::eventsNames[current.type];
}

EventHandler::operator Events() {
    if (raised) {
        sync.wait();
        raised = false;
        fired = false;
    }
    return current.type;
}

void EventHandler::initHooks() {
    HookSubscriber<WH_KEYBOARD_LL>::init();
    HookSubscriber<WH_MOUSE_LL>::init();
    FocusSubscriber::init();
}

/* Subscibtion methods */
void EventHandler::onMouseUp(Button btn, Event event, Flags flags) {
    addTrigger(msTriggers, State::up, btn, event, flags);
}

void EventHandler::onMouseDown(Button btn, Event event, Flags flags) {
    addTrigger(msTriggers, State::down, btn, event, flags);
}

void EventHandler::onKeyUp(Key btn, Event event, Flags flags) {
    addTrigger(kbTriggers, State::up, btn, event, flags);
}

void EventHandler::onKeyDown(Key btn, Event event, Flags flags) {
    addTrigger(kbTriggers, State::down, btn, event, flags);
}

void EventHandler::onBlur(Events evType) {
    fcTriggers[Focus::blur].push_back(evType);
}

void EventHandler::onFocus(Events evType) {
    fcTriggers[Focus::focus].push_back(evType);
}

void EventHandler::watchAppFocus(const char* path) {
    applicationPath = path;
}

/* Windows hook callback handlers */
template <class btnSearch_t, class Button_t>
void EventHandler::addTrigger(std::map<btnSearch_t, triggerList_t>& triggersMap, State state, Button_t btn, Event event, Flags flags) {
    auto searchPair = std::make_pair(btn, state);
    auto it = triggersMap.find(searchPair);

    if (it == triggersMap.end()) {
        triggerList_t list = { std::make_pair(event, flags) };
        triggersMap.emplace(searchPair, std::move(list));
    }
    else {
        it->second.emplace_back(std::make_pair(event, flags));
    }
}

template <class btnSearch_t, class Button_t, class Data_t>
void EventHandler::handleFires(std::map<btnSearch_t, triggerList_t> triggerMap, Button_t btn, State state, Data_t data) {
    auto it = triggerMap.find(std::make_pair(btn, state));
    if (it != triggerMap.end()) {
        for (const auto& trigger : it->second) {
            Flags flags = trigger.second;
            if ((flags & Flags::notInjected) && (data.flags & LLKHF_INJECTED)) continue;
            //if ((flags & Flags::scriptActive) && isScriptRunning())      continue;

            EventHandler::Event a = trigger.first;
            fire(trigger.first);
        }
    }
}

void EventHandler::mouseCallback(WPARAM action, LPARAM lParam) {
    Mouse::mouseEvent msev = Mouse::identifyEvent(action);
    if (msev != Mouse::mouseEvent::button) return; //ignore all events except buttons press

    auto data = *reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

    //if (data.flags == LLMHF_INJECTED) return;

    auto buttonPair = Mouse::identifyButton(action, data);
    handleFires(msTriggers, buttonPair.first, buttonPair.second, data);
}

void EventHandler::keyboardCallback(WPARAM action, LPARAM lp) {
    auto data = *reinterpret_cast<KBDLLHOOKSTRUCT*>(lp);

    Key key = Key(data.vkCode);
    State state = (action == WM_KEYDOWN || action == WM_SYSKEYDOWN)
        ? State::down
        : State::up;

    handleFires(kbTriggers, key, state, data);
}

void EventHandler::focusCallback(const char* path) {
    if (applicationPath == nullptr) return;

    Focus now = ((strcmp(applicationPath, path) == 0) ? Focus::focus : Focus::blur);
    std::list<Events>& trigList = fcTriggers[now];

    for (const auto& trigger : trigList) {
        fire(trigger);
    }
}

std::map<EventHandler::Events, std::string>
EventHandler::eventsNames = {
      { EventHandler::Events::app_focused,    "App focused"    },
      { EventHandler::Events::app_blured,     "App blured"     },
      { EventHandler::Events::script_restart, "Script restart" },
      { EventHandler::Events::script_action,  "Script action"  },
      { EventHandler::Events::script_stop,    "Script stop"    },
      { EventHandler::Events::script_start,   "Script start"   },
};