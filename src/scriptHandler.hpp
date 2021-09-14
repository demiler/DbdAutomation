#pragma once
#include "./eventHandler.hpp"
#include "./soundHandler.hpp"
#include "./script.hpp"
#include <spdlog/spdlog.h>
#include <functional>

class ScriptHandler {
public:
    enum class Scripts;
    ScriptHandler() : script(nullptr),
                      callback(std::bind(&ScriptHandler::scriptEnded, this))
    {}

    ~ScriptHandler() {
        if (isRunning()) {
            script->stop();
            //scriptEnded() will be called as callback
        }
    }

    bool isRunning() { return script != nullptr; }

    void start(Scripts sc) {
        if (isRunning()) return;

        script = ScriptHandler::createScript(sc);
        script->setEndCallback(callback);
        script->start();
        sh.play(SoundHandler::Sounds::script_started);
    }

    void stop() {
        if (!isRunning()) return;
        script->stop();
        //scriptEnded(); will be called as callback
    }

    void restart() {
        if (!isRunning()) return;
        script->restart();
        sh.play(SoundHandler::Sounds::script_started);
    }

    void action() {
        if (!isRunning()) return;
        script->action();
    }

    bool hasAction() {
        if (!isRunning()) return false;
        return script->hasAction();
    }

private:
    SoundHandler sh;
    std::function<void(void)> callback;
    static Script* createScript(Scripts script);
    Script* script;

    void scriptEnded() {
        delete script;
        script = nullptr;
        spdlog::info("Script ended");
        sh.play(SoundHandler::Sounds::script_ended);
    }
};

EventHandler::Event ScriptStart(ScriptHandler::Scripts script) {
    return EventHandler::Event(EventHandler::Events::script_start, static_cast<unsigned>(script));
}

//move to separte file (smth like, scripts_creation.cpp)
enum class ScriptHandler::Scripts { wiggle, autogen, toxic };

Script* ScriptHandler::createScript(ScriptHandler::Scripts script) {
    switch (script) {
        case Scripts::toxic: return new BecomeToxic;
        case Scripts::autogen: return new Autogen;
        case Scripts::wiggle: return new Wiggle;
        default:
            spdlog::warn("Attempt to create unknown script type");
            return nullptr;
    }
}
