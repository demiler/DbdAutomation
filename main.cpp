#include "./src/scripts/ScriptsRegister.hpp"
#include "./src/keyboardHandler.hpp"
#include "./src/mouseHandler.hpp"
#include "./src/eventHandler.hpp"
#include "./src/scriptHandler.hpp"
#include "./src/soundHandler.hpp"
#include "./src/exceptions.hpp"
#include "./src/overlay.hpp"
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
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    spdlog::info("Message loop is completed");
}

void readConfig(EventHandler &event) {
    try {
        event.onKeyUp(Key::q, Events::exit);
        event.onKeyUp(Key::z, ScriptStart(Scripts::autogen));
        event.onKeyDown(Key::ctrl, Events::script_stop, Flags::notInjected);
        event.onKeyDown(Key::shift, Events::script_stop, Flags::notInjected);

        event.onMouseDown(Button::right, Events::script_action, Flags::notInjected);
        event.onMouseDown(Button::middle, Events::script_restart, Flags::scriptActive);
        event.onMouseDown(Button::forward, ScriptStart(Scripts::wiggle));
        event.onMouseDown(Button::backward, ScriptStart(Scripts::toxic));

        event.onBlur(Events::app_blured);
        event.onFocus(Events::app_focused);
        event.watchAppFocus("D:\\Games\\steamapps\\common\\Dead by Daylight\\DeadByDaylight\\Binaries\\Win64\\DeadByDaylight-Win64-Shipping.exe");
    }
    catch (winapiError winerr) {
        spdlog::error("On initialization winapi error occured\nCode: {}\nWhat: {}", winerr.code(), winerr.what());
        exit(winerr.code());
    }
}

void EventLoop() {
    EventHandler event;
    readConfig(event);

    SoundHandler sound;
    ScriptHandler script;
    Scripts scrType;
    bool focused = false;

    sound.play(Sounds::app_open);

    while (event != Events::exit) {
try {
        switch (event) {
            case Events::app_focused:
                spdlog::info("App focused");
                focused = true;
                Overlay::show();
                sound.play(Sounds::app_focused);
                break;

            case Events::app_blured:
                spdlog::info("App blured");
                script.stop();
                focused = false;
                Overlay::hide();
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
                if (focused && !script.isRunning()) {
                    spdlog::info("Starting script {}", sts(scrType));
                    script.start(scrType);
                }
                else if (focused) {
                    spdlog::info("Can't start script: {} is already running", sts(scrType));
                }
                else {
                    spdlog::info("Can't start script: App is not in focus");
                }
                break;

            default:
                spdlog::warn("Unknown event: {}", event.getEventName());
        }
        event.raise();
} catch (winapiError winerr) {
    spdlog::error("In event loop winapi error occured\nCode: {}\nWhat: {}", winerr.code(), winerr.what());
} catch (std::exception err) {
    spdlog::error("In event loop something bad happened\nWhat: {}", err.what());
}
    }

    sound.play(Sounds::app_close);
    spdlog::info("Closing...");
    exit(0);
}

int main(void) {
    try {
        EventHandler::initHooks();
        Overlay::init();
    }
    catch (winapiError winerr) {
        spdlog::error("Winapi error occurred on hook init\nCode: {}\nWhat: {}\n", winerr.code(), winerr.what());
        return winerr.code();
    }
    catch (std::exception err) {
        spdlog::error("Something bad happened on hook init\nWhat: {}", err.what());
        return -1;
    }

    auto eventThread = std::thread(EventLoop);
    msgLoop();
    eventThread.join();
    return 0;
}