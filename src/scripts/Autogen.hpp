#pragma once
#include "../script.hpp"
#include "../bitmap.hpp"
#include "../utils.hpp"
#include <Windows.h>

struct coord {
    int x, y;
};

class Autogen : public Script {
    static Bitmap image;
    Button btn;

    static const int STEP;
    static const double RTW; //radis to width constant

    int tickX, tickY;
    bool tickFound;

    coord coords[360];
    
    HWND gameWin;
    HDC gameDC;
    millis_t tickTimeout;

    bool isTickColor(COLORREF color) { return color == 0xffffff; }
public:
    Autogen() {
        deathTime = 60s;
        warningTime = 25s;
    }

    bool cancelButtonsPressed() {
        return kb[Key::ctrl] == State::down || kb[Key::shift] == State::down;
    }

    bool startLoop() {
        if (cancelButtonsPressed()) Sleep(500);
        return !cancelButtonsPressed();
    }

    void beforeLoop() {
        btn = (ms[Button::right] == State::down) ? Button::right : Button::left;
        ms.push(btn);
        ms.lock(btn); //lock - dont send button action to the system

        tickFound = false;
        gameWin = GetForegroundWindow();
        if (gameWin == NULL) {
            spdlog::error("Couln't get foreground window");
            return;
        }

        gameDC = GetDC(gameWin);
        if (gameDC == NULL) {
            spdlog::error("Couldn't get window DC");
            return;
        }

        RECT gameRect;
        GetWindowRect(gameWin, &gameRect);
        int width = gameRect.right - gameRect.left;
        int height = gameRect.bottom - gameRect.top;

        if (GetWindowLong(gameWin, GWL_STYLE) & WS_BORDER) {
            height -= 23; //substract title bar height
        }

        int offsetX = width / 2;
        int offsetY = height / 2;
        int radius = width * Autogen::RTW;
        tickTimeout = 0ms;

        for (int i = 359; i >= 0; i -= Autogen::STEP) {
            coords[i].x = offsetX + radius * cos(degToRad(i));
            coords[i].y = offsetY + radius * sin(degToRad(i));
        }
    }

    void loopAction() {
        kb.press(Key::space, 30);
        tickFound = false;
    }

    void afterLoop() {
        ms.unlock(btn);
        ms.release(btn);

        ReleaseDC(gameWin, gameDC);
    }

    void loop() {
        for (int i = 359; !tickFound && i >= 0; i -= Autogen::STEP) {
            tickX = coords[i].x;
            tickY = coords[i].y;

            COLORREF color = GetPixel(gameDC, tickX, tickY);
            if (isTickColor(color)) {
                //std::cout << "Tick found" << std::endl;
                tickFound = true;
                tickTimeout = millis();
            }
        }
        if (tickFound) {
            COLORREF color = GetPixel(gameDC, tickX, tickY);

            if (!isTickColor(color)) {
                //std::cout << "Tick triggered " << std::endl;
                kb.press(Key::space, 30);
                tickFound = false;
                Sleep(500);
            }
            else if (millis() - tickTimeout > 1.8s) {
                //std::cout << "Tick timeout" << std::endl;
                tickFound = false;
            }
        }
        //Sleep(20);
    }

    bool hasAction()  override { return true; }
    bool hasImage()   override { return true; }
    Bitmap getImage() override { return Autogen::image; }
};

Bitmap Autogen::image(IMG_AUTOGEN);
const int Autogen::STEP = 5;
//standart radius to width ratio based on mesaurment on 1920 resolution
const double Autogen::RTW = (66.0 / 1920.0);
