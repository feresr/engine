#pragma once

#include "Subtexture.h"
#include <string>
#include <vector>

namespace Engine {

    struct Frame {
        Engine::Subtexture texture;
        int durationMillis;
    };


    struct Animation {
        std::string name;
        std::vector<Frame> frames;
        int duration;
    };

    class Sprite {

    public:

        Sprite() = delete;

        Sprite(const std::string &&name) : name{name} {}

        glm::vec2 pivot{};

        Animation *getAnimation() {
            return &animations[0];
        }
        Animation *getAnimation(const std::string &name) {
            if (name.empty()) return &animations[0];
            for (auto &animation : animations) {
                if (animation.name == name) return &animation;
            }
            return nullptr;
        }

        Animation &addAnimation() {
            return animations.emplace_back();
        }

        std::string name;
    private:

        std::vector<Animation> animations;

    };

}
