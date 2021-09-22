#pragma once
#include "./eventHandler.hpp"
#include "./soundHandler.hpp"
#include "./script.hpp"
#include "./overlay.hpp"
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
        if (script->playSounds()) sh.play(SoundHandler::Sounds::script_started);
        if (script->changeIndicator()) Overlay::setIndicatorColor(0x7850da);
        if (script->hasImage()) Overlay::setImage(script->getImage());
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
        if (script->playSounds()) sh.play(SoundHandler::Sounds::script_ended);
        if (script->changeIndicator()) Overlay::setIndicatorColor(25, 100, 50);
        if (script->hasImage()) Overlay::clearImage();
        delete script;
        script = nullptr;
        spdlog::info("Script ended");
    }
};

EventHandler::Event ScriptStart(ScriptHandler::Scripts script) {
    return EventHandler::Event(EventHandler::Events::script_start, static_cast<unsigned>(script));
}