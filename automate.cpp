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
