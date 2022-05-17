#include "time/time.h"
#include "SDL.h"

uint32_t Engine::Time::pauseTimer = 0;

bool Engine::Time::onInterval(uint32_t interval) {
    auto currentTimeMillis = SDL_GetTicks();

    auto d = currentTimeMillis / interval;

    return d % 2 == 0;
}

uint32_t Engine::Time::getTime() {
    return SDL_GetTicks();
}
