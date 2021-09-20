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

template <class T, class U>
T inRange(T value, U min, U max) {
	if (value > max) return max;
	if (value < min) return min;
	return value;
}

millis_t millis();


int random(int from, int to);

void getPathByHWND(HWND hwnd, char processPath[MAX_PATH]);