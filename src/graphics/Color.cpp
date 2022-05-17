#include "Color.h"

namespace Engine {

    Color::Color() : r(0), g(0), b(0), a(0) {}

    Color::Color(int rgb) :
            r((uint8_t) ((rgb & 0xFF0000) >> 16)),
            g((uint8_t) ((rgb & 0x00FF00) >> 8)),
            b((uint8_t) (rgb & 0x0000FF)),
            a(255) {}

    Color::Color(int rgb, float alpha) :
            r((int) (((uint8_t) ((rgb & 0xFF0000) >> 16)) * alpha)),
            g((int) (((uint8_t) ((rgb & 0x00FF00) >> 8)) * alpha)),
            b((int) (((uint8_t) (rgb & 0x0000FF)) * alpha)),
            a((int) (255 * alpha)) {}

    Color::Color(uint8_t r, uint8_t g, uint8_t b)
            : r(r), g(g), b(b), a(255) {}

    Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
            : r(r), g(g), b(b), a(a) {}

    uint32_t Color::toRGBA() const {
        return ((uint32_t) r << 24) |
               ((uint32_t) g << 16) |
               ((uint32_t) b << 8) |
               (uint32_t) a;
    }

    Color::Color(const Color& other) {
        this->r = other.r;
        this->g = other.g;
        this->b = other.b;
        this->a = other.a;
    }


}