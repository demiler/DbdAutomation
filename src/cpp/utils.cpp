#include "../utils.hpp"
#include "../exceptions.hpp"
#include <spdlog/spdlog.h>
#include <psapi.h>

/* can't separte template declaration and implementation
template<typename R>
bool isFutureReady(std::future<R> const& f) {
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}*/

millis_t millis() {
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
}

int random(int from, int to) {
    return rand() % (to - from + 1) + from;
}

void getPathByHWND(HWND hwnd, char processPath[MAX_PATH]) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (processHandle != NULL) {
        if (!GetModuleFileNameEx(processHandle, NULL, processPath, MAX_PATH)) {
            spdlog::error("Unable to get process path");
            throw winapiError("Unable to get process path");
        }
        CloseHandle(processHandle);
    }
    else {
        spdlog::error("Unable to open process");
        throw winapiError("Unable to open process");
    }
}