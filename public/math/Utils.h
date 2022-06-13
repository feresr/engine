#pragma once


#include <algorithm>
#include <glm/glm.hpp>

namespace Engine::Math {

    float approach(float value, float target, float delta);

    glm::vec2 approach(glm::vec2 value, glm::vec2 target, float delta);

    constexpr float cubeInOut(float x) {
        return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
    }

    glm::mat3x2 transform(const glm::vec2 &position, const glm::vec2 &origin, const glm::vec2 &scale, float radians);
    glm::mat3x2 transform(const glm::vec2 &position, const glm::vec2 &origin, const glm::vec2 &scale);
    glm::mat3x2 rotate(float radians);
    glm::mat3x2 scale(glm::vec2 scale);
}
