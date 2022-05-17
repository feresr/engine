#pragma once

#include <cstdint>

namespace Engine {
    struct Time {

        static uint32_t pauseTimer;

        static bool onInterval(uint32_t interval);

        static uint32_t getTime();

    };
}
