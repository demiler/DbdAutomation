#pragma once
#include <Windows.h>
#include <chrono>
#include <future>
#include <spdlog/spdlog.h>
#include "../resource.h"
#include "utitls.hpp"
using namespace std::chrono_literals;

class soundResource_t {
    HGLOBAL res;
public:
    soundResource_t() : res(NULL) {}
    ~soundResource_t() { if (assigned()) FreeResource(res); }
    bool assigned() { return res != NULL; }
    operator LPCSTR() { return reinterpret_cast<LPCSTR>(res); }
    
    void init(int SND_RES) {
        if (assigned()) return;
        HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(SND_RES), "WAVE");
        if (hResInfo == NULL) {
            spdlog::error("Failed to find resource");
            throw winapiError("Failed to find resource");
        }

        res = LoadResource(NULL, hResInfo);
        if (res == NULL) {
            spdlog::error("Failed to load resource");
            throw winapiError("Falied to load resource");
        }
    }
};

class SoundHandler {
public:
    enum class Sounds {
        app_open, app_close,
        app_focused, app_blured,
        script_started, script_warning, script_ended
    };

    SoundHandler() {
        SoundHandler::beepIn.init(SND_BEEPIN);
        SoundHandler::beepOut.init(SND_BEEPOUT);
        SoundHandler::warning.init(SND_WARNING);
    }

    void beep(const std::list<std::pair<int, millis_t>>& beeps) {
        beepAsync = std::async(std::launch::async, [&beeps]() {
            for (const auto& snd : beeps)
                Beep(snd.first, snd.second.count());
        });
    }

    void beep(const std::list<std::pair<int, millis_t>>&& beeps) {
        beepAsync = std::async(std::launch::async, [beeps = std::move(beeps)]() {
            for (const auto& snd : beeps)
                Beep(snd.first, snd.second.count());
        });
    }

    void beep(int freq, millis_t duration, int count = 1) {
        beepAsync = std::async(std::launch::async, [](int freq, millis_t duration, int count) {
            while (count > 0) {
                Beep(freq, duration.count());
                count--;
            }
        }, freq, duration, count);
    }

    void play(Sounds sound) {
        switch (sound) {
            case Sounds::script_started:
                beep(400, 200ms, 1);
                //PlaySound(beepIn, NULL, SND_MEMORY | SND_ASYNC);
                break;
            case Sounds::script_warning:
                beep(400, 130ms, 2);
                //PlaySound(warning, NULL, SND_MEMORY | SND_ASYNC);
                break;
            case Sounds::script_ended:
                beep(400, 100ms, 3);
                //PlaySound(beepOut, NULL, SND_MEMORY | SND_ASYNC);
                break;
            case Sounds::app_open:
                beep(750, 300ms, 2);
                break;
            case Sounds::app_close:
                beep(350, 100ms, 3);
                break;
            case Sounds::app_focused:
                beep({ {886, 100ms}, {1827, 100ms}, {3615, 100ms} });
                break;
            case Sounds::app_blured:
                beep({ {3615, 100ms}, {1827, 100ms}, {886, 100ms} });
                break;
        }
    }

private:
    std::future<void> beepAsync;
    static soundResource_t beepIn, beepOut, warning;
};

soundResource_t SoundHandler::beepIn;
soundResource_t SoundHandler::beepOut;
soundResource_t SoundHandler::warning;