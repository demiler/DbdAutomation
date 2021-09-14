#pragma once
#include <exception>
#include <Windows.h>
#include <string>

class winapiError : public std::exception {
    std::string __what, __more;
    DWORD __code;
public:
    winapiError(std::string &what) : __code(GetLastError()), __what(what) {}
    winapiError(const char *what) : __code(GetLastError()), __what(what) {}
    winapiError(DWORD code, const std::string &what) : __code(code), __what(what) {}
    winapiError(DWORD code, const char *what) : __code(code), __what(what) {}

    const std::string& what() { return __what; }
    DWORD code() { return __code; }

    const std::string& more() {
        if (!__more.empty()) return __more;

        LPSTR messageBuffer = nullptr;

        //Ask Win32 to give us the string version of that message ID.
        //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, __code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

        __more = std::string(messageBuffer, size);
        LocalFree(messageBuffer);
        return __more;
    }
};