#pragma once
#include "../script.hpp"
#include "../bitmap.hpp"

class Wiggle : public Script {
    static Bitmap image;
public:
    Wiggle() { deathTime = 30s; warningTime = 25s; }
    void loop() {
        kb.press(Key::a, 30);
        kb.press(Key::d, 30);
    }

    bool hasImage() { return true; }
    Bitmap getImage() { return Wiggle::image; }
};
Bitmap Wiggle::image(IMG_WIGGLE);