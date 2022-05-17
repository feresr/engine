#pragma once

#include <utility>
#include "KeyCodes.h"

namespace Engine {
    enum Mouse {
        BUTTON_LEFT = 1,
        BUTTON_MIDDLE = 2,
        BUTTON_RIGHT = 3,
    };

    class Input {
    public:
        static bool down(Engine::Key key);
        static bool pressed(Engine::Key key);

        static bool pressed(Engine::Mouse button);

        static float getMouseX();

        static float getMouseY();

        static std::pair<float, float> getMousePosition();

        static void update();
    };

}