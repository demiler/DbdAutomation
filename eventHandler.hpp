#pragma once
#include <map>
#include <string>
#include <future>

class EventHandler {
public:
    enum class Events {
		unset,
        app_focused, app_blured, script_restart,
        script_action, script_stop, script_start,
		exit,
    };

	struct Event {
		EventHandler::Events type;
		unsigned value;
		Event() : type(EventHandler::Events::unset), value(0u) {}
		Event(const Event& ev) : type(ev.type), value(ev.value) {}
		Event(EventHandler::Events type, unsigned value = 0u) : type(type), value(value) {}
	};

    EventHandler(bool raise = true) : fired(false) {
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
		return current.type;
    }

    unsigned getValue() {
        return current.value;
    }

    const std::string& getEventName() {
        return EventHandler::eventsNames[current.type];
    }

    void fire(const Event &event) {
        if (raised && !fired) {
            sync_prom.set_value();
            fired = true;
        }
        current = event;
    }

private:
    EventHandler(const EventHandler&) = delete;
    EventHandler(EventHandler&&) = delete;

    static std::map<Events, std::string> eventsNames;
	Event current;
    std::promise<void> sync_prom;
    std::future<void> sync;
    bool raised, fired;
};

std::map<EventHandler::Events, std::string>
EventHandler::eventsNames = {
      { EventHandler::Events::app_focused,   "App focused"    },
      { EventHandler::Events::app_blured,    "App blured"     },
      { EventHandler::Events::script_restart, "Script restart" },
      { EventHandler::Events::script_action, "Script action"  },
      { EventHandler::Events::script_stop,   "Script stop"  },
      { EventHandler::Events::script_start,  "Script start"   },
};

