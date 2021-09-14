#pragma once
#include "./keyboardHandler.hpp"
#include "./mouseHandler.hpp"
#include "./eventHandler.hpp"
#include "./utitls.hpp"
#include <spdlog/spdlog.h>
#include <thread>
#include <future>
#include <chrono>
#include <functional>
#include <iostream>

using Key = Keyboard::Key;
using Button = Mouse::Button;
using Events = EventHandler::Events;
using namespace std::chrono_literals;

class Script {
public:
    void start() {
        resetPromise();
        loopThread = std::thread(&Script::looping, this);
        loopThread.detach();
    }

    void restart() {
        loopPromise.set_value(Events::script_restart);
    }

    void stop() {
        loopPromise.set_value(Events::script_stop);
    }

    void action() {
        loopPromise.set_value(Events::script_action);
    }

    void setEndCallback(std::function<void(void)> cb) {
        endCallback = cb;
    }

    virtual bool hasAction() { return false; }

protected:
    Mouse ms;
    Keyboard kb;
    millis_t startTime, deathTime;

    virtual void beforeLoop() {};
    virtual void afterLoop() {};
    virtual void loopAction() {};
    virtual void loop() = 0;
    virtual bool startLoop() { return true; }
    virtual bool isTimeout() { return millis() - startTime >= deathTime; }

private:
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
        bool running = true;

        beforeLoop();
        while (running) {
            loop();

            if (isReady(loopFuture)) {
                switch (loopFuture.get()) {
                    case Events::script_stop:
                        running = false;
                        break;
                    case Events::script_action:
                        loopAction();
                        break;
                    case Events::script_restart:
                        startTime = millis();
                        break;
                    default:
                        spdlog::error("Unkown event in script loop");
                        ;// throw Exeption(ExType::unknown_event, "Unkown event in script loop");
                }
                resetPromise();
            }

            if (isTimeout())
                running = false;
        }
        afterLoop();
        endCallback();
    }
};

class Wiggle : public Script {
public:
    Wiggle() { deathTime = 1000ms; }
    void loop() {
        kb.press(Key::a, 30);
        kb.press(Key::d, 30);
    }
};

class Autogen : public Script {
public:
    bool cancelButtonsPressed() {
        return kb[Key::ctrl] == State::down || kb[Key::shift] == State::down;
    }

    bool startLoop() {
        if (cancelButtonsPressed()) Sleep(500);
        return !cancelButtonsPressed();
    }

    void beforeLoop() {
        ms.push(Button::left);
        //ms.lock(Button::left); //lock - dont send button action to the system
    }

    void loopAction() {
        kb.press(Key::space, 30);
    }
    
    void afterLoop() {
        ms.release(Button::left);
        //ms.unlock(Button::left);
    }

    void loop() { Sleep(10); }

    bool hasAction() override { return true; }
};

class BecomeToxic : public Script {
    enum class Type { tbag, click };
    Type type;
public:
    BecomeToxic() {
        deathTime = 1000ms;
        type = Type::tbag;
    }

    bool startLoop() {
        return kb[Key::ctrl] == State::up && kb[Key::shift] == State::up;
    }

    void beforeLoop() {
        type = (ms[Button::left] == State::down) ? Type::click : Type::tbag;
        if (type == Type::tbag) {
            kb.press(Key::ctrl, 300);
        }
    }

    void loop() {
        if (type == Type::tbag) {
            kb.push(Key::ctrl);
            Sleep(40);
            kb.release(Key::ctrl);
            Sleep(40);
        }
        else {
            ms.push(Button::left);
            Sleep(40);
            ms.release(Button::left);
            Sleep(40);
        }
    }

    bool isTimeout() {
        return Script::isTimeout() && ms[Button::left] != State::down;
    }
};