#pragma once
#include <Windows.h>
#include <map>
#include <algorithm>
#include <spdlog/spdlog.h>
#include "./exceptions.hpp"

class Bitmap {
public:
    Bitmap() : resource(NULL), bitmapID(NULL) {}

    Bitmap(unsigned bitmapID, unsigned size = 64) : size(size), bitmapID(bitmapID) {
        auto& refPair = Bitmap::refCounts[bitmapID];

        if (refPair.first == 0) {
            HANDLE handle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(bitmapID), IMAGE_BITMAP, size, size, NULL);
            if (handle == NULL) {
                spdlog::error("Can't load image with id {}", bitmapID);
                throw winapiError("Failed to load bitmap");
            }
            resource = static_cast<HBITMAP>(handle);
            refPair.first = 1;
            refPair.second = resource;
        }
        else {
            refPair.first++;
            resource = refPair.second;
        }
    }
    
    Bitmap(const Bitmap& bm) : size(bm.size), resource(bm.resource), bitmapID(bm.bitmapID) {
        Bitmap::refCounts[bitmapID].first++;
    }
    
    Bitmap(Bitmap&& bm) : resource(NULL), size(bm.size), bitmapID(bm.bitmapID) {
        std::swap(resource, bm.resource);
    }

    ~Bitmap() {
        clear();
    }

    Bitmap& operator=(Bitmap&& bm) {
        clear();
        size = bm.size;
        bitmapID = bm.bitmapID;
        std::swap(resource, bm.resource);
        return *this;
    }

    Bitmap& operator=(const Bitmap& bm) {
        clear();
        resource = bm.resource;
        bitmapID = bm.bitmapID;
        size = bm.size;
        Bitmap::refCounts[bitmapID].first++;
        return *this;
    }

    Bitmap& operator=(int null) {
        clear();
        return *this;
    }

    bool isSet() { return resource != NULL; }

    HBITMAP get() {
        return resource;
    }

    void draw(HDC hdc, int x = 5, int y = 5) {
        BITMAP bm;
        HDC hdcMem = CreateCompatibleDC(hdc);
        HGDIOBJ oldBitmap = SelectObject(hdcMem, resource);

        GetObject(resource, sizeof(bm), &bm);
        BitBlt(hdc, x, y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);
    }

private:
    void clear() {
        if (isSet() && --Bitmap::refCounts[bitmapID].first == 0) {
            DeleteObject(static_cast<HGDIOBJ>(resource));
        }
        resource = NULL;
    }

    HBITMAP resource;
    unsigned size, bitmapID;
    static std::map<unsigned, std::pair<unsigned, HBITMAP>> refCounts;
};

std::map<unsigned, std::pair<unsigned, HBITMAP>> Bitmap::refCounts;