#pragma once
#include "./eventHandler.hpp"
#include "./script.hpp"

class ScriptHandler {
public:
    enum class Scripts { wiggle, autogen, toxic };
    ScriptHandler() : script(nullptr) {}

    void start(Scripts sc) {
        if (script == nullptr) {
            script = ScriptHandler::createScript(sc);
            script->start();
        }
    }

    void stop() {
        if (script != nullptr) {
            script->stop();
            delete script;
            script = nullptr;
        }
    }

    void restart() {
        script->restart();
    }

    void action() {
        script->action();
    }

private:
    static Script* createScript(Scripts script);

    friend struct INIT;
    Script *script;
};

Script* ScriptHandler::createScript(ScriptHandler::Scripts script) {
    switch (script) {
        //case Scripts::autogen: return new Autogen;
        //case Scripts::toxic: return new BecomeToxic;
        case Scripts::wiggle: return new Wiggle;
        default:
            spdlog::warn("Attempt to create unknown script type");
            return nullptr;
    }
}

EventHandler::Event ScriptStart(ScriptHandler::Scripts script) {
    return EventHandler::Event(EventHandler::Events::script_start, static_cast<unsigned>(script));
}