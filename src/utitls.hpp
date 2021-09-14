#pragma once
#include <chrono>
#include "./exceptions.hpp"

typedef std::chrono::milliseconds millis_t;

template<typename R>
bool isFutureReady(std::future<R> const& f) {
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

millis_t millis() {
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
}

void getPathByHWND(HWND hwnd, char processPath[MAX_PATH]) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (processHandle != NULL) {
        if (!GetModuleFileNameEx(processHandle, NULL, processPath, MAX_PATH))
            throw winapiError("Unable to get process path");
        CloseHandle(processHandle);
    }
    else {
        throw winapiError("Unable to open process");
    }
}