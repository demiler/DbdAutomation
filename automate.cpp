#include <iostream>
#include <chrono>
#include <thread>
#include <future>

class Script {
private:
    void deathTimer()
    {
        auto timerFuture = deathTimerPromise.get_future();
        auto curWaitTime = warningTime;
        bool running = true;
        bool warned = false;

        while (running) {
            running = false;
            auto outcome = timerFuture.wait_for(curWaitTime);

            //if outcome is ready it means that we either restarted script or stoped it
            if (outcome == std::future_status::ready) {
                warned = false;
                curWaitTime = warningTime;

                running = timerFuture.get();

                deathTimerPromise = std::promise<bool>();
                timerFuture = deathTimerPromise.get_future();

                //if (running) makeRestartSound()
            }
            //otherwise we either have wated whole curWaitTime
            else if (outcome == std::future_status::timeout && !warned) {
                //keep thread running for another N time
                curWaitTime = deathTime - warningTime;
                running = true;
                warned = true;
                //makeWarningSound()
            }
        }
    }

protected:
    std::chrono::milliseconds deathTime, warningTime, loopTimeout;
    bool playSound;

    std::promise<bool> deathTimerPromise;
    std::future<void> timerThread, loopFuture;

    inline bool isRunning() {
        return timerThread.wait_for(loopTimeout) != std::future_status::ready;
    }

public:
    Script() = delete;
    Script(
        std::chrono::milliseconds deathTime,
        std::chrono::milliseconds warningTime,
        std::chrono::milliseconds loopTimeout,
        bool playSound = true)
        :
        deathTime(deathTime),
        warningTime(warningTime),
        loopTimeout(loopTimeout),
        playSound(playSound)
    {
        if (deathTime <= warningTime)
            throw "Death time could not be less or equal warning time";
    }

    virtual void start()
    {
        timerThread = std::async(std::launch::async, &Script::deathTimer, this);
        loopFuture = std::async(std::launch::async, &Script::loop, this);
        //makeStartSound()
        //notifyAboutStart()
    }

    virtual void stop()
    {
        deathTimerPromise.set_value(false);
    }

    virtual void restart()
    {
        deathTimerPromise.set_value(true);
    }

    virtual void loop()
    {
        do {
            loopAction();
        } while (isRunning());
    }

    virtual void action() {}

    virtual void loopAction() {}
};

class Struggle : public Script {
public:
    using Script::Script;
    void loopAction() override
    {
        //pressKey();
    }
};

class Wiggle : public Script {
public:
    using Script::Script;
    void loopAction() override
    {
        //pressKey();
        //pressKey();
    }
};

class BecomeToxic : public Script {
    std::chrono::milliseconds tBagTime, clickTime;
    bool tBag;
public:
    BecomeToxic(
        std::chrono::milliseconds deathTime,
        std::chrono::milliseconds warningTime,
        std::chrono::milliseconds loopTimeout,
        std::chrono::milliseconds tBagTime,
        std::chrono::milliseconds clickTime,
        bool playSound = false)
        :
        Script(deathTime, warningTime, loopTimeout, playSound),
        tBagTime(tBagTime),
        clickTime(clickTime) {}

    void start() override
    {
        //if (shift.isHeld == true) return;
        //tBag = leftMouseButton.isHeld()
        if (tBag)
            ;//ctrl.holdFor(125ms);
        Script::start();
    }

    void loop() override
    {
        auto pushTime = tBag ? tBagTime : clickTime;
        //auto pushFunction = tBag ? ctrl.press : leftMouseButton.press;
        //auto releaseFunction = tBag ? ctrl.release : leftMouseButton.release;
        do {
            //pushFunction();
            //sleep(pushTime);
            //releaseFunction();
            //sleep(pushTime);
        } while (isRunning() );//&& leftMouseButton.isHeld());
    }
};

class AutoGen : public Script {
public:
    using Script::Script;

    void start() override
    {
        //if (isAnyOfCancellButtonsPressed()) sleep(300ms);
        //if (isAnyOfCancellButtonsPressed()) return;
        Script::start();
    }

    void loop() override
    {
        //leftMouseButton.press()
        while (isRunning());
        //leftMouseButton.release()
    }

    void action() override
    {
        //pressKey
    }
};
