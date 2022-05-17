#include "Input.h"
#include "Application.h"

namespace Engine {

    Uint8 previousKeyboard[SDL_NUM_SCANCODES]{0};

    void Input::update() {
        memcpy(&previousKeyboard, SDL_GetKeyboardState(nullptr), SDL_NUM_SCANCODES);
    }

    bool Input::down(Key key) {
        auto* state = SDL_GetKeyboardState(nullptr);
        return state[key];
    }

    bool Input::pressed(Key key) {
        auto* state = SDL_GetKeyboardState(nullptr);
        return state[key] && !previousKeyboard[key];
    }

    bool Input::pressed(Engine::Mouse button) {
        return SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(button);
    }

    std::pair<float, float> Input::getMousePosition() {
        int x, y;
        SDL_GetMouseState(&x, &y);

        return std::pair(x, y);
    }

    float Input::getMouseX() {
        return (float) getMousePosition().first;
    }

    float Input::getMouseY() {
        return (float) getMousePosition().second;
    }
}
