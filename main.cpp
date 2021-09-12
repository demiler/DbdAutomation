#include "./src/keyboardHandler.hpp"
#include "./src/mouseHandler.hpp"
#include "./src/eventHandler.hpp"
#include "./src/hookSubscriber.hpp"
#include "./src/scriptHandler.hpp"
#include "spdlog/spdlog.h"
#include <thread>
#include <iostream>

using Scripts = ScriptHandler::Scripts;
using Events = EventHandler::Events;
using Button = Mouse::Button;
using Key = Keyboard::Key;

auto sts(Scripts script) {
    switch (script) {
        case Scripts::autogen: return "autogen";
        case Scripts::toxic: return "toxic";
        case Scripts::wiggle: return "wiwggle";
        default:
            spdlog::warn("Attempt to create unknown script type");
            return "<unknown>";
    }
}

void msgLoop() {
    spdlog::info("Started message loop");
	MSG logit;
	GetMessage(&logit, NULL, 0, 0);
    spdlog::info("Message loop is completed");
}

void EventLoop() {
    EventHandler event;

	event.onKeyUp(Keyboard::Key::q, Events::exit);
    //event.onkeyup(  Key::z,     Start(Scripts::autogen));
    event.onKeyDown(Key::ctrl,  Events::script_stop, Flags::notInjected);
    event.onKeyDown(Key::shift, Events::script_stop, Flags::notInjected);

    event.onMouseDown(Button::right,    Events::script_action,  Flags::scriptActive);
    event.onMouseDown(Button::middle,   Events::script_restart, Flags::scriptActive);
    //event.onMouseDown(Button::middle,   ScriptStart(Scripts::struggle));
    event.onMouseDown(Button::forward,  ScriptStart(Scripts::wiggle));
    //event.onMouseDown(Button::Backward, Script(Scripts::toxic));

    //SoundHandler sound;
    ScriptHandler script;
    bool focused = false;
    while (event != Events::exit) {
        switch (event) {
            case Events::app_focused:
                spdlog::info("App focused");
                //focused = true;
                //sound.make(Sounds::app_focused)
                break;

            case Events::app_blured:
                spdlog::info("App blured");
                //script.stop();
                //focused = false;
                //sound.make(Sounds::app_blured);
                break;

            case Events::script_restart:
                spdlog::info("Script restarted");
                //script.restart();
                break;

            case Events::script_stop:
                spdlog::info("Script stopped");
                script.stop();
                break;

            case Events::script_action:
                //spdlog::info("script_action");
                //script.action();
                break;

            case Events::script_start: {
                    Scripts scrType = Scripts(event.getValue());
                    spdlog::info("Starting script {}", sts(scrType));
                    script.start(scrType);
                }
                break;

            default:
				spdlog::warn("Unknown event: {}", event.getEventName());
        }
        event.raise();
	}
    exit(0);
}

int main(void) {
    EventHandler::initHooks();
	auto eventThread = std::thread(EventLoop);
	msgLoop();
	eventThread.join();
	return 0;
}