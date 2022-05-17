#include <cmath>
#include <Utils.h>

namespace Engine::Math
{

    float approach(float value, float target, float delta)
    {
        return value < target ? fmin(value + delta, target) : fmax(value - delta, target);
    }

    glm::vec2 approach(glm::vec2 value, glm::vec2 target, float delta)
    {
        if (glm::length(target - value) <= delta)
            return target;
        return value + glm::normalize(target - value) * delta;
    }

    glm::mat3x2 rotate(float radians)
    {
        auto c = cos(radians);
        auto s = sin(radians);
        return glm::mat3x2(c, s, -s, c, 0, 0);
    }

    glm::mat3x2 transform(const glm::vec2 &position, const glm::vec2 &origin, const glm::vec2 &scale, float radians) {
        auto matrix = glm::mat3x2{1.0, 0.0, 0.0, 1.0, 0.0, 0.0};

        if (origin.x != 0 || origin.y != 0)
        {
            glm::mat3x2 transform{1, 0.0f, 0.0f, 1, -origin.x, -origin.y};
            matrix = transform;
        }
        if (scale.x != 1 || scale.y != 1)
        {
            glm::mat3x2 transform{
                scale.x, 0.0f,
                0.0f, scale.y,
                0.0f, 0.0f};
            matrix = transform * glm::mat3x3(matrix);
        }
        
        if (radians != 0) {
            glm::mat3x2 transform = rotate(radians);
            matrix = transform * glm::mat3x3(matrix);
        }

        if (position.x != 0 || position.y != 0)
        {
            glm::mat3x2 transform{
                1.0, 0.0f,
                0.0f, 1.0,
                position.x,
                position.y};

            matrix = transform * glm::mat3x3(matrix);
        }

        return matrix;
    }
    glm::mat3x2 transform(const glm::vec2 &position, const glm::vec2 &origin, const glm::vec2 &scale)
    {
        return transform(position, origin, scale, 0.0f);
    }

}