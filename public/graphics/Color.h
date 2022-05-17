#pragma once

#include <cinttypes>

namespace Engine {

    struct Color {

        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        Color();

        Color(int rgb);

        Color(const Color& other);

        Color(int rgb, float alpha);

        [[nodiscard]] uint32_t toRGBA() const;

        Color(uint8_t r, uint8_t g, uint8_t b);

        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    };
}