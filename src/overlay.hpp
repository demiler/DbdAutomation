#pragma once
#include <Windows.h>
#include <spdlog/spdlog.h>
#include "exceptions.hpp"
#include "utils.hpp"

class Overlay {
public:
    static void init() {
        if (initialized) return;
        Overlay::initialized = true;
        
        try {
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

    /*
    static void drawImage() {
        hdcMem = CreateCompatibleDC(hdc);
        oldBitmap = SelectObject(hdcMem, hBitmap);

        GetObject(hBitmap, sizeof(bitmap), &bitmap);
        BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);
    }
    */

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
                //hBitmap = (HBITMAP)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 200, 200, LR_LOADTRANSPARENT);
                //if (hBitmap == NULL) std::cout << "Fail: " << GetLastError() << std::endl;
                return TRUE;

            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc;
                RECT rc;

                hdc = BeginPaint(hwnd, &ps);
                
                GetClientRect(hwnd, &rc);
                SetDCBrushColor(hdc, RGB(0,0,0));
                FillRect(hdc, &rc, (HBRUSH)GetStockObject(DC_BRUSH));

                SelectObject(hdc, GetStockObject(DC_PEN));
                SelectObject(hdc, GetStockObject(DC_BRUSH));

                SetDCBrushColor(hdc, Overlay::circleColor);
                SetDCPenColor(hdc, Overlay::circleColor);
                Ellipse(hdc, 0, 0, 40, 40);
                
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
};

const char Overlay::className[] = "DBDOverlay";
HCURSOR Overlay::cursor   = LoadCursor(NULL, IDC_SIZEALL);
const int Overlay::width  = 40;
const int Overlay::height = 40;
int Overlay::alpha		  = 100;
HBITMAP Overlay::imgs     = NULL;
HWND Overlay::hwnd 		  = NULL;
bool Overlay::initialized = false;
COLORREF Overlay::circleColor = RGB(25, 100, 50);