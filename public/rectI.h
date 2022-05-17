#pragma once

#include "glm/glm.hpp"

namespace Engine {


    struct RectI {
        int x;
        int y;
        int w;
        int h;

        RectI();

        RectI(int rx, int ry, int rw, int rh);

        RectI(glm::ivec2 pos, glm::ivec2 size);

        RectI scale(int s);

        RectI scale(int sx, int sy);

        [[nodiscard]] int left() const;

        [[nodiscard]] int right() const;

        [[nodiscard]] int top() const;

        [[nodiscard]] int bottom() const;

        [[nodiscard]] glm::ivec2 center() const;

        [[nodiscard]] int center_x() const;

        [[nodiscard]] int center_y() const;

        [[nodiscard]] glm::ivec2 top_left() const;

        [[nodiscard]] glm::ivec2 top_right() const;

        [[nodiscard]] glm::ivec2 bottom_right() const;

        [[nodiscard]] glm::ivec2 bottom_left() const;

        [[nodiscard]] glm::ivec2 center_left() const;

        [[nodiscard]] glm::ivec2 center_right() const;

        [[nodiscard]] glm::ivec2 middle_top() const;

        [[nodiscard]] glm::ivec2 middle_bottom() const;

        [[nodiscard]] bool contains(const glm::ivec2 &pt) const;

        [[nodiscard]] bool overlaps(const RectI &rect) const;

        [[nodiscard]] RectI overlap_rect(const RectI &other) const;

        [[nodiscard]] glm::ivec2 size() const;

        /*
            RectI Sectors:
                0101  0100  0110
                0001  0000  0010
                1001  1000  1010
            0000 = inside rectangle, all others refer to sectors relative to the rectangle
        */
        char get_sector(const glm::ivec2 &pt) const;

        bool operator==(const RectI &rhs) const { return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h; }

        bool operator!=(const RectI &rhs) const { return !(*this == rhs); }

        RectI operator+(const glm::ivec2 &rhs) const;

        RectI operator-(const glm::ivec2 &rhs) const;

        RectI &operator+=(const glm::ivec2 &rhs);

        RectI &operator-=(const glm::ivec2 &rhs);

        // moves this rect to fit inside 'other' 
        void fitInto(const RectI& other) {
            // fit horizontally
            this->x = (std::fmax) ((float) this->left(), other.left());
            this->x = (std::fmin) ((float) this->right(), other.right()) - this->size().x;
            // fit horizontally
            this->y = (std::fmax) ((float) this->bottom(), other.bottom()) - this->size().y;
            this->y = (std::fmin) ((float) this->top(), other.top());
        }
    };
}
