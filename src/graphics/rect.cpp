#include "rect.h"

using namespace Engine;

Rect::Rect() {
    x = y = w = h = 0;
};

Rect::Rect(float rx, float ry, float rw, float rh) {
    x = rx;
    y = ry;
    w = rw;
    h = rh;
}

Rect::Rect(glm::vec2 pos, glm::vec2 size) {
    x = pos.x;
    y = pos.y;
    w = size.x;
    h = size.y;
}

Rect Rect::scale(float s) {
    x = (x * s);
    y = (y * s);
    w = (w * s);
    h = (h * s);
    return *this;
}

Rect Rect::scale(float sx, float sy) {
    x = (x * sx);
    y = (y * sy);
    w = (w * sx);
    h = (h * sy);
    return *this;
}

float Rect::left() const {
    return x;
}

float Rect::right() const {
    return x + w;
}

float Rect::top() const {
    return y;
}

float Rect::bottom() const {
    return y + h;
}

glm::vec2 Rect::center() const {
    return glm::vec2(x + w / 2, y + h / 2);
}

float Rect::center_x() const {
    return x + w / 2;
}

float Rect::center_y() const {
    return y + h / 2;
}

glm::vec2 Rect::top_left() const {
    return glm::vec2(x, y);
}

glm::vec2 Rect::top_right() const {
    return glm::vec2(x + w, y);
}

glm::vec2 Rect::bottom_right() const {
    return glm::vec2(x + w, y + h);
}

glm::vec2 Rect::bottom_left() const {
    return glm::vec2(x, y + h);
}

glm::vec2 Rect::center_left() const {
    return glm::vec2(x, y + h / 2);
}

glm::vec2 Rect::center_right() const {
    return glm::vec2(x + w, y + h / 2);
}

glm::vec2 Rect::middle_top() const {
    return glm::vec2(x + w / 2, y);
}

glm::vec2 Rect::middle_bottom() const {
    return glm::vec2(x + w / 2, y + h);
}

bool Rect::contains(const glm::vec2& pt) const {
    return pt.x >= x && pt.x < x + w && pt.y >= y && pt.y < y + h;
}

bool Rect::overlaps(const Rect& rect) const {
    return x + w >= rect.x && y + h >= rect.y && x < rect.x + rect.w && y < rect.y + rect.h;
}

Rect Rect::overlap_rect(const Rect& against) const {
    Rect result(0, 0, 0, 0);

    if (x + w >= against.x && x < against.x + against.w) {
        result.x = glm::max(x, against.x);
        result.w = glm::min(x + w, against.x + against.w) - result.x;
    }

    if (y + h >= against.y && y < against.y + against.h) {
        result.y = glm::max(y, against.y);
        result.h = glm::min(y + h, against.y + against.h) - result.y;
    }

    return result;
}

Rect Rect::operator+(const glm::vec2& rhs) const {
    return Rect(x + rhs.x, y + rhs.y, w, h);
}


Rect Rect::operator-(const glm::vec2& rhs) const { return Rect(x - rhs.x, y - rhs.y, w, h); }

Rect& Rect::operator+=(const glm::vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Rect& Rect::operator-=(const glm::vec2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

char Rect::get_sector(const glm::vec2& pt) const {
    char h;
    if (pt.x < left())
        h = 0b0001;
    else if (pt.x >= right())
        h = 0b0010;
    else
        h = 0;

    char v;
    if (pt.y < top())
        v = 0b0100;
    else if (pt.y >= bottom())
        v = 0b1000;
    else
        v = 0;

    return h | v;
}

glm::vec2 Rect::size() const {
    return glm::vec2(w, h);
}
