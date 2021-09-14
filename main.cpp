#include "./src/keyboardHandler.hpp"
#include "./src/mouseHandler.hpp"
#include "./src/eventHandler.hpp"
#include "./src/scriptHandler.hpp"
#include "./src/soundHandler.hpp"
#include "spdlog/spdlog.h"
#include <thread>
#include <iostream>

using Scripts = ScriptHandler::Scripts;
using Sounds = SoundHandler::Sounds;
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

    event.onKeyUp(  Key::q,     Events::exit);
    event.onKeyUp(  Key::z,     ScriptStart(Scripts::autogen));
    event.onKeyDown(Key::ctrl,  Events::script_stop, Flags::notInjected);
    event.onKeyDown(Key::shift, Events::script_stop, Flags::notInjected);

    event.onMouseDown(Button::right,    Events::script_action,  Flags::scriptActive);
    event.onMouseDown(Button::middle,   Events::script_restart, Flags::scriptActive);
    event.onMouseDown(Button::forward,  ScriptStart(Scripts::wiggle));
    event.onMouseDown(Button::backward, ScriptStart(Scripts::toxic));
    
    event.onBlur(Events::app_blured);
    event.onFocus(Events::app_focused);
    event.watchAppFocus("C:\\Program Files (x86)\\Notepad++\\notepad++.exe");

    SoundHandler sound;
    ScriptHandler script;
    Scripts scrType;
    bool focused = false;

    sound.play(Sounds::app_open);

    while (event != Events::exit) {
        switch (event) {
            case Events::app_focused:
                spdlog::info("App focused");
                focused = true;
                sound.play(Sounds::app_focused);
                break;

            case Events::app_blured:
                spdlog::info("App blured");
                script.stop();
                focused = false;
                sound.play(Sounds::app_blured);
                break;

            case Events::script_restart:
                if (script.isRunning()) {
                    spdlog::info("Script restarted");
                    script.restart();
                }
                break;

            case Events::script_stop:
                if (script.isRunning()) {
                    spdlog::info("Stopping script");
                    script.stop();
                }
                break;

            case Events::script_action:
                if (script.hasAction()) {
                    spdlog::info("Script Action");
                    script.action();
                }
                break;

            case Events::script_start: 
                scrType = Scripts(event.getValue());
                if (!script.isRunning()) {
                    spdlog::info("Starting script {}", sts(scrType));
                    script.start(scrType);
                }
                else {
                    spdlog::info("Can't start script: {} is already running", sts(scrType));
                }
                break;

            default:
                spdlog::warn("Unknown event: {}", event.getEventName());
        }
        event.raise();
    }
    sound.play(Sounds::app_close);
    spdlog::info("Closing...");
    exit(0);
}

int main(void) {
    EventHandler::initHooks();
    auto eventThread = std::thread(EventLoop);
    msgLoop();
    eventThread.join();
    return 0;
}