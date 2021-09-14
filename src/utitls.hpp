#pragma once
#include <chrono>
#include <future>
#include <Windows.h>

typedef std::chrono::milliseconds millis_t;

//Can't separte template declaration and implementation
template<typename R>
bool isFutureReady(std::future<R> const& f) {
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

millis_t millis();


int random(int from, int to);

void getPathByHWND(HWND hwnd, char processPath[MAX_PATH]);