#pragma once
#include "./keyboardHandler.hpp"
#include "./mouseHandler.hpp"
#include "./eventHandler.hpp"
#include <spdlog/spdlog.h>
#include <thread>
#include <future>
#include <chrono>

using Key = Keyboard::Key;
using Button = Mouse::Button;
using Events = EventHandler::Events;

template<typename R>
bool isReady(std::future<R> const& f) {
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

time_t millis() {
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}
class Script {
public:
    void start() {
        resetPromise();
        loopThread = std::thread(&Script::looping, this);
    }

    void restart() {
        loopPromise.set_value(Events::script_restart);
    }

    void stop() {
        loopPromise.set_value(Events::script_stop);
        loopThread.join();
    }

    void action() {
        loopPromise.set_value(Events::script_action);
    }

protected:
    std::thread loopThread;
    std::promise<Events> loopPromise;
    std::future<Events> loopFuture;
    time_t startTime, deathTime;

    Mouse ms;
    Keyboard kb;

    virtual void beforeLoop() {};
    virtual void afterLoop() {};
    virtual void loopAction() {};
    virtual void loop() = 0;
    virtual bool startLoop() { return true; }
    virtual bool isTimeout() { return startTime - millis() >= deathTime; }

private:
    void resetPromise() {
        loopPromise = std::promise<Events>();
        loopFuture = loopPromise.get_future();
    }

    void looping() {
        if (!startLoop()) return;

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
    }
};

class Wiggle : public Script {
public:
    void loop() {
        kb.press(Key::a, 30);
        kb.press(Key::d, 30);
    }
};

/*
class Autogen : public Script {
public:
    bool startLoop() {
        if (kb[Key::ctrl] == State::down || kb[Key::shift] == State::down) {
            Sleep(500);
        }
        return kb[Key::ctrl] == State::up && kb[Key::shift] == State::up;
    }

    void beforeLoop() {
        ms.push(Button::left);
        ms.lock(Button::left); //lock - dont send button action to the system
    }

    void loopAction() {
        kb.press(Key::space, 30);
    }
    
    void afterLoop() {
        ms.release(Button::left);
        ms.unlock(Button::left);
    }
};

class Toxic : public Script {
    enum class Type { tbag, click };
    Type type;
public:
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
*/