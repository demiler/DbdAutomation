#include <map>
#include <string>
#include <future>

struct Event {
    EventHandler::Events type;
    unsigned value;
    Event(EventHandler::Events type, unsigned value = 0) : type(type), value(value) {}
};

class EventHandler {
public:
    enum class Events {
        app_focused, app_blured, scrip_restart,
        script_action, script_stop, script_start,
    };

    EventHandler(bool raise = false) : fired(false) {
        if (raise) this->raise();
        else raised = false;
    }

    void raise() {
        raised = true;
        sync_prom = std::promise<void>();
        sync = sync_prom.get_future();
    }

    operator Events() {
        if (raised) {
            sync.wait();
            raised = false;
            fired = false;
        }
        return current;
    }

    void setValue(const std::string &newValue) {
        value = newValue;
    }

    const std::string& getValue() {
        return value;
    }

    const std::string& getEventName() {
        return EventHandler::eventsNames[current];
    }

    void fire(Events event) {
        fire(event, "");
    }

    void fire(Events event, const std::string &value) {
        if (raised && !fired) {
            sync_prom.set_value();
            fired = true;
        }
        current = event;
        this->value = value;
    }

private:
    EventHandler(const EventHandler&) = delete;
    EventHandler(EventHandler&&) = delete;

    static std::map<Events, std::string> eventsNames;
    std::string value;
    Events current;
    std::promise<void> sync_prom;
    std::future<void> sync;
    bool raised, fired;
};

std::map<EventHandler::Events, std::string>
EventHandler::eventsNames = {
      { EventHandler::Events::app_focused,   "App focused"    },
      { EventHandler::Events::app_blured,    "App blured"     },
      { EventHandler::Events::scrip_restart, "Script restart" },
      { EventHandler::Events::script_action, "Script action"  },
      { EventHandler::Events::script_stop,   "Script stop"  },
      { EventHandler::Events::script_start,  "Script start"   },
};
