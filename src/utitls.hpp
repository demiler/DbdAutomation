#pragma once
#include <chrono>

typedef std::chrono::milliseconds millis_t;

template<typename R>
bool isReady(std::future<R> const& f) {
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

millis_t millis() {
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
}