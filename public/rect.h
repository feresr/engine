#pragma once

#include "glm/glm.hpp"

namespace Engine {

    struct Rect {
        float x;
        float y;
        float w;
        float h;

        Rect();

        Rect(float rx, float ry, float rw, float rh);

        Rect(glm::vec2 pos, glm::vec2 size);

        Rect scale(float s);

        Rect scale(float sx, float sy);

        [[nodiscard]] float left() const;

        [[nodiscard]] float right() const;

        [[nodiscard]] float top() const;

        [[nodiscard]] float bottom() const;

        [[nodiscard]] glm::vec2 center() const;

        [[nodiscard]] float center_x() const;

        [[nodiscard]] float center_y() const;

        [[nodiscard]] glm::vec2 top_left() const;

        [[nodiscard]] glm::vec2 top_right() const;

        [[nodiscard]] glm::vec2 bottom_right() const;

        [[nodiscard]] glm::vec2 bottom_left() const;

        [[nodiscard]] glm::vec2 center_left() const;

        [[nodiscard]] glm::vec2 center_right() const;

        [[nodiscard]] glm::vec2 middle_top() const;

        [[nodiscard]] glm::vec2 middle_bottom() const;

        [[nodiscard]] bool contains(const glm::vec2& pt) const;

        [[nodiscard]] bool overlaps(const Rect& rect) const;

        [[nodiscard]] Rect overlap_rect(const Rect& other) const;

        [[nodiscard]] glm::vec2 size() const;

        /*
            Rect Sectors:
                0101  0100  0110
                0001  0000  0010
                1001  1000  1010
            0000 = inside rectangle, all others refer to sectors relative to the rectangle
        */
        char get_sector(const glm::vec2& pt) const;

        bool operator==(const Rect& rhs) const { return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h; }

        bool operator!=(const Rect& rhs) const { return !(*this == rhs); }

        Rect operator+(const glm::vec2& rhs) const;

        Rect operator-(const glm::vec2& rhs) const;

        Rect& operator+=(const glm::vec2& rhs);

        Rect& operator-=(const glm::vec2& rhs);

    };
}
