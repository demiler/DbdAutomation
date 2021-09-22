#pragma once
#include "./keyboardHandler.hpp"
#include "./mouseHandler.hpp"
#include "./eventHandler.hpp"
#include "./utils.hpp"
#include "./soundHandler.hpp"
#include "./overlay.hpp"
#include <spdlog/spdlog.h>
#include <thread>
#include <future>
#include <chrono>
#include <functional>
#include <iostream>
#include <exception>

using Key = Keyboard::Key;
using Button = Mouse::Button;
using Events = EventHandler::Events;
using namespace std::chrono_literals;

class Script {
public:
    Script() : deathTime(0), warningTime(0) {}

    void start() {
        resetPromise();
        loopThread = std::thread(&Script::looping, this);
        loopThread.detach();
    }

    void restart() {
        //loopPromise.set_value(Events::script_restart);
        setPromiseValue(Events::script_restart);
    }

    void stop() {
        //loopPromise.set_value(Events::script_stop);
        setPromiseValue(Events::script_stop);
    }

    void action() {
        //loopPromise.set_value(Events::script_action);
        setPromiseValue(Events::script_action);
    }

    void setEndCallback(std::function<void(void)> cb) {
        endCallback = cb;
    }

    virtual bool hasAction() { return false; }
    virtual bool playSounds() { return true; }
    virtual bool changeIndicator() { return true; }
    virtual bool hasImage() { return false; }
    virtual Bitmap getImage() { return Bitmap(); }

protected:
    Mouse ms;
    Keyboard kb;
    millis_t startTime, deathTime, warningTime, runTime;

    virtual void beforeLoop() {};
    virtual void afterLoop() {};
    virtual void loopAction() {};
    virtual void warning() { Overlay::blink(0x21ffaa, 200ms, 3); }
    virtual void loop() = 0;
    virtual bool startLoop() { return true; }
    virtual bool isTimeout() { return runTime >= deathTime; }

private:
    inline bool hasWarning() { return warningTime == 0ms; }

    void setPromiseValue(Events event) {
        try {
            loopPromise.set_value(event);
        }
        catch (...) {
            spdlog::error("Something bad happen of set_value in script. Event: {}", static_cast<int>(event));
        }
    }

    std::thread loopThread;
    std::promise<Events> loopPromise;
    std::future<Events> loopFuture;
    std::function<void(void)> endCallback;

    void resetPromise() {
        loopPromise = std::promise<Events>();
        loopFuture = loopPromise.get_future();
    }

    void looping() {
        if (!startLoop()) {
            endCallback();
            return;
        }

        startTime = millis();
        runTime = 0ms;
        bool running = true;
        bool warned = hasWarning(); //ignore warning if warningTime is not set

        beforeLoop();
        while (running) {
            loop();

            if (isFutureReady(loopFuture)) {
                switch (loopFuture.get()) {
                    case Events::script_stop:
                        running = false;
                        break;
                    case Events::script_action:
                        loopAction();
                        break;
                    case Events::script_restart:
                        startTime = millis();
                        warned = hasWarning();
                        break;
                    default:
                        throw std::invalid_argument("Unkown event in script loop");
                }
                resetPromise();
            }

            runTime = millis() - startTime;
            if (!warned && runTime >= warningTime) {
                warned = true;
                warning();
            }

            if (isTimeout())
                running = false;
        }
        afterLoop();
        endCallback();
    }
};