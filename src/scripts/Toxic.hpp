#pragma once
#include "../script.hpp"
#include "../bitmap.hpp"

class BecomeToxic : public Script {
    static Bitmap image;
    enum class Type { tbag, click };
    Type type;
public:
    BecomeToxic() {
        deathTime = 500ms;
        type = Type::tbag;
    }

    bool playSounds() override { return false; }

    bool startLoop() {
        return kb[Key::ctrl] == State::up && kb[Key::shift] == State::up;
    }

    void beforeLoop() {
        type = (ms[Button::left] == State::down) ? Type::click : Type::tbag;
        if (type == Type::tbag) {
            kb.press(Key::ctrl, 200);
        }
    }

    void loop() {
        if (type == Type::tbag) {
            kb.push(Key::ctrl);
            Sleep(50);
            kb.release(Key::ctrl);
            Sleep(50);
        }
        else {
            ms.push(Button::right);
            Sleep(10);
            ms.release(Button::right);
            Sleep(random(10, 40));
        }
    }

    bool isTimeout() {
        return Script::isTimeout() && ms[Button::left] != State::down;
    }

    bool hasImage() override { return true; }
    Bitmap getImage() override { return BecomeToxic::image; }
};
Bitmap BecomeToxic::image(IMG_TOXIC);