#pragma once
#include <Windows.h>
#include <spdlog/spdlog.h>
#include <future>
#include "exceptions.hpp"
#include "bitmap.hpp"
#include "utils.hpp"
#include "../resource.h"

HBITMAP hBitmap;

class Overlay {
public:
    static void init() {
        if (initialized) return;
        Overlay::initialized = true;
        
        try {
            Overlay::asyncBlink = std::async(std::launch::async, [](){});
            Overlay::registerClass();
            Overlay::createWindow();

            SetLayeredWindowAttributes(Overlay::hwnd, 0x000000, Overlay::alpha, LWA_COLORKEY | LWA_ALPHA);
            ShowWindow(Overlay::hwnd, SW_HIDE);
            UpdateWindow(Overlay::hwnd);
        }
        catch (winapiError err) {
            throw err;
        }
    }
    
    static void pointerEvents(bool clickable) {		
        long dwExStyle = GetWindowLong(Overlay::hwnd, GWL_EXSTYLE);
        if (clickable) dwExStyle &= ~WS_EX_TRANSPARENT;
        else dwExStyle |= WS_EX_TRANSPARENT;
        SetWindowLong(Overlay::hwnd, GWL_EXSTYLE, dwExStyle);
    }

    static void show() {
        ShowWindow(Overlay::hwnd, SW_SHOW);
    }

    static void hide() {
        ShowWindow(Overlay::hwnd, SW_HIDE);
    }

    static void setIndicatorColor(int hex) {
        Overlay::circleColor = hex;
        InvalidateRect(Overlay::hwnd, NULL, NULL);
    }

    static void setIndicatorColor(int red, int green, int blue) {
        Overlay::circleColor = RGB(red, green, blue);
        InvalidateRect(Overlay::hwnd, NULL, NULL);
    }

    static void setAlpha(int alpha) {
        Overlay::alpha = alpha;
        SetLayeredWindowAttributes(Overlay::hwnd, 0x000000, Overlay::alpha, LWA_COLORKEY | LWA_ALPHA);
    }

    static COLORREF getIndicatorColor() {
        return Overlay::circleColor;
    }

    static int getAlpha() {
        return Overlay::alpha;
    }

    static void moveTo(int x, int y) {
        MoveWindow(Overlay::hwnd, x, y, Overlay::width, Overlay::height, FALSE);
    }

    static void setImage(const Bitmap& bm) {
        Overlay::bitmap = bm;
        InvalidateRect(Overlay::hwnd, NULL, NULL);
    }

    static void clearImage() {
        Overlay::bitmap = NULL;
        InvalidateRect(Overlay::hwnd, NULL, NULL);
    }

    static bool blink(COLORREF color, millis_t delay, unsigned times) {
        if (!isFutureReady(Overlay::asyncBlink)) return false;

        asyncBlink = std::async(std::launch::async,
            [](COLORREF color, millis_t delay, unsigned times) {
                COLORREF oldColor = circleColor;
                while (times > 0) {
                    Overlay::setIndicatorColor(color);
                    Sleep(delay.count());
                    Overlay::setIndicatorColor(oldColor);
                    Sleep(delay.count());
                    times--;
                }
            },
            color, delay, times);
        return true;
    }

    static void PrintHello() {
        HDC dc = GetDC(Overlay::hwnd);
        RECT rc;
        GetClientRect(Overlay::hwnd, &rc);
        DrawText(dc, "Hello!", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        ReleaseDC(Overlay::hwnd, dc);
    }

private:
    static void registerClass() {
        WNDCLASSEX winClass    = { 0 };
        winClass.style         = CS_HREDRAW | CS_VREDRAW;
        winClass.cbSize        = sizeof(WNDCLASSEX);
        winClass.lpfnWndProc   = Overlay::windowHandler;
        winClass.hInstance     = GetModuleHandle(NULL);
        winClass.lpszClassName = Overlay::className;
        winClass.hCursor	   = LoadCursor(nullptr, IDC_SIZEALL);
        winClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        
        if (RegisterClassEx(&winClass) == NULL) {
            spdlog::error("Failed to register class");
            throw winapiError("Failed to register class");
        }
    }
    
    static void createWindow() {
        Overlay::hwnd = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST, //Release version
            //WS_EX_LAYERED,
            Overlay::className,
            NULL,
            WS_POPUP,// | WS_OVERLAPPEDWINDOW,
            1500, 0,
            Overlay::width, Overlay::height,
            GetConsoleWindow(),
            NULL,
            GetModuleHandle(NULL),
            NULL
        );
        
        if (Overlay::hwnd == NULL) {
            spdlog::error("Failed to create window");
            throw winapiError("Failed to create window");
        }
    }

    static LRESULT CALLBACK windowHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        switch (msg) {
            case WM_MOVING: {//restrict overlay from going beyond screen
                RECT* lpRect = (RECT*)(lp);
                lpRect->left   = inRange(lpRect->left, 0, 1920 - Overlay::width);
                lpRect->top    = inRange(lpRect->top, 0, 1080 - Overlay::height);
                lpRect->right  = lpRect->left + Overlay::width;
                lpRect->bottom = lpRect->top + Overlay::height;
                return TRUE;
            }

            case WM_SETCURSOR: {
                SetCursor(Overlay::cursor);
                return TRUE;
            }

            case WM_NCHITTEST: //make window dragable
                return HTCAPTION;

            case WM_SETTEXT:
                return TRUE;

            case WM_NCCREATE:
                return TRUE;

            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc;
                RECT rc;

                hdc = BeginPaint(hwnd, &ps);
                
                GetClientRect(hwnd, &rc);
                SetDCBrushColor(hdc, RGB(0, 0, 0));
                FillRect(hdc, &rc, (HBRUSH)GetStockObject(DC_BRUSH));

                if (Overlay::bitmap.isSet()) Overlay::bitmap.draw(hdc);

                SelectObject(hdc, GetStockObject(DC_PEN));
                SelectObject(hdc, GetStockObject(DC_BRUSH));

                SetDCBrushColor(hdc, Overlay::circleColor);
                SetDCPenColor(hdc, Overlay::circleColor);
                Ellipse(hdc, 0, 0, Overlay::circleSize, Overlay::circleSize);
                
                EndPaint(hwnd, &ps);
                return 0;
            }
        }
        return DefWindowProc(hwnd, msg, wp, lp);
    }

    static int alpha;
    static const int width, height;
    static const char className[];
    static HCURSOR cursor;
    static HBITMAP imgs;
    static HWND hwnd;
    static bool initialized;
    static COLORREF circleColor;
    static const int circleSize;
    static Bitmap bitmap;
    static std::future<void> asyncBlink;
};

const char Overlay::className[] = "DBDOverlay";
HCURSOR Overlay::cursor   = LoadCursor(NULL, IDC_SIZEALL);
const int Overlay::width  = 70;
const int Overlay::height = 70;
int Overlay::alpha		  = 100;
HBITMAP Overlay::imgs     = NULL;
HWND Overlay::hwnd 		  = NULL;
bool Overlay::initialized = false;
const int Overlay::circleSize = 20;
COLORREF Overlay::circleColor = RGB(25, 100, 50);
Bitmap Overlay::bitmap;
std::future<void> Overlay::asyncBlink;