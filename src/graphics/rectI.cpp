#include "rectI.h"

using namespace Engine;

RectI::RectI() {
    x = y = w = h = 0;
};


RectI::RectI(int rx, int ry, int rw, int rh) {
    x = rx;
    y = ry;
    w = rw;
    h = rh;
}

RectI::RectI(glm::ivec2 pos, glm::ivec2 size) {
    x = pos.x;
    y = pos.y;
    w = size.x;
    h = size.y;
}

RectI RectI::scale(int s) {
    x = (x * s);
    y = (y * s);
    w = (w * s);
    h = (h * s);
    return *this;
}

RectI RectI::scale(int sx, int sy) {
    x = (x * sx);
    y = (y * sy);
    w = (w * sx);
    h = (h * sy);
    return *this;
}

int RectI::left() const {
    return x;
}

int RectI::right() const {
    return x + w;
}

int RectI::top() const {
    return y;
}

int RectI::bottom() const {
    return y + h;
}

glm::ivec2 RectI::center() const {
    return glm::vec2(x + w / 2, y + h / 2);
}

int RectI::center_x() const {
    return x + w / 2;
}

int RectI::center_y() const {
    return y + h / 2;
}

glm::ivec2 RectI::top_left() const {
    return glm::vec2(x, y);
}

glm::ivec2 RectI::top_right() const {
    return glm::vec2(x + w, y);
}

glm::ivec2 RectI::bottom_right() const {
    return glm::vec2(x + w, y + h);
}

glm::ivec2 RectI::bottom_left() const {
    return glm::vec2(x, y + h);
}

glm::ivec2 RectI::center_left() const {
    return glm::vec2(x, y + h / 2);
}

glm::ivec2 RectI::center_right() const {
    return glm::vec2(x + w, y + h / 2);
}

glm::ivec2 RectI::middle_top() const {
    return glm::vec2(x + w / 2, y);
}

glm::ivec2 RectI::middle_bottom() const {
    return glm::vec2(x + w / 2, y + h);
}

bool RectI::contains(const glm::ivec2 &pt) const {
    return pt.x > x && pt.x <= x + w && pt.y > y && pt.y <= y + h;
}

bool RectI::overlaps(const RectI &rect) const {
    return x + w > rect.x && y + h > rect.y && x < rect.x + rect.w && y < rect.y + rect.h;
}

RectI RectI::overlap_rect(const RectI &against) const {
    RectI result(0, 0, 0, 0);

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

RectI RectI::operator+(const glm::ivec2 &rhs) const {
    return RectI(x + rhs.x, y + rhs.y, w, h);
}


RectI RectI::operator-(const glm::ivec2 &rhs) const { return RectI(x - rhs.x, y - rhs.y, w, h); }

RectI &RectI::operator+=(const glm::ivec2 &rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

RectI &RectI::operator-=(const glm::ivec2 &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

char RectI::get_sector(const glm::ivec2 &pt) const {
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

glm::ivec2 RectI::size() const {
    return glm::vec2(w, h);
}

