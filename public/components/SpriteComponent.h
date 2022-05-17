#pragma once

#include <string>
#include "Component.h"

namespace Engine {

    class Sprite;
    class Animation;

    class SpriteComponent : public Component {

    public:
        SpriteComponent() = delete;

        explicit SpriteComponent(const std::string &sprite);

        void play(const std::string &animation);

        void render(Batch &batch) override;

        void update() override;

        glm::vec2 scale{1.0f, 1.0f};

        int getCurrentAnimDuration();

        // todo: getCurrentAnimSize doesn't work becaue the packer packs alpha values
        glm::ivec2 getCurrentAnimSize();

        bool awake() override;

    private:
        int frameIndex{0};
        float frameCounter{0};

        std::string spriteName;
        std::string animationName;

        Engine::Sprite* getSprite();
        Engine::Animation* getAnimation();
    };
}